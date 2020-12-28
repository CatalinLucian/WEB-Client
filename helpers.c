#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "buffer.h"
#include "parson.h"
#include "requests.h"

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void compute_message(char *message, const char *line)
{
    strcat(message, line);
    strcat(message, "\r\n");
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    /* connect the socket */
    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}

void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);

        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;

            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);

            if (content_length_start < 0) {
                continue;
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;

    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *basic_extract_json_response(char *str)
{
    return strstr(str, "{\"");
}

char* get_cookie(char* response) {
  char* cookie;
  if (response == NULL) {
    return NULL;
  }
  char* token = strtok(response, "\n");
  while (token) {
    // find cookie in the server response
    if (strstr(token, "connect.sid") != NULL) {
      cookie = strtok(token, " ;");
      cookie = strtok(NULL, ";");
      return cookie;
    }
    token = strtok(NULL, "\n");
  }
  return NULL;
}

char* register_command(char* username, char* password) {
  // creates the JSON to be transmited for the command
  char* key[50] = {"username", "password"};
  char* value[50] = {username, password};
  char* register_credentials = createJSON(key, value, 2);

  // initiates connection to the server
  int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
  // creates the POST message
  char* message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                               "/api/v1/tema/auth/register", register_credentials, NULL, 0, NULL);
  // sends the message to the server
  send_to_server(sockfd, message);
  // gets response back
  char* response = receive_from_server(sockfd);
  // if I get no message back then registration worked
  if (basic_extract_json_response(response) == NULL) {
    printf("Inregistrarea s-a realizat cu succes\n");
  } else {
    /// otherwise something bad happened and prints the error
    printf("%s\n", basic_extract_json_response(response));
  }
  // closes the connection
  close_connection(sockfd);
  return NULL;
}

char* login_command(char* username, char* password) {
  // creates the JSON to be transmited for the command
  char* key[50] = {"username", "password"};
  char* authentication[50] = {username, password};
  char* login_credentials = createJSON(key, authentication, 2);

  // open the connection
  int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
  // computes the POST request with the JSON creates as data to be posted
  char* message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                  "/api/v1/tema/auth/login", login_credentials, NULL, 0, NULL);
  // sends the message to the server
  send_to_server(sockfd, message);
  // gets response back
  char* response = receive_from_server(sockfd);
  if (basic_extract_json_response(response) == NULL) {
    printf("Autentificarea s-a realizat cu succes\n");
  } else {
    printf("%s\n", basic_extract_json_response(response));
  }
  close_connection(sockfd);
  // the needed part from the response is the coookie
  char* cookie = get_cookie(response);
  return cookie;
}

char* enter_library(char* cookie) {
  // opens the connection
  int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
  // computes the GET request at the given path
  char* message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                       "/api/v1/tema/library/access", NULL, cookie, 0, NULL);
  // sends message to server
  send_to_server(sockfd, message);
  char* response = receive_from_server(sockfd);
  close_connection(sockfd);
  // parse the response to get the JWT token
  char* token = strstr(response, "{\"token");
  char* jwt = strtok(token, ":");
  jwt = strtok(NULL, "\"");
  if (jwt != NULL) {
    printf("Succes\n");
  } else {
    printf("Eroare\n");
  }
  // I will use the JWT token to demonstrate i have access to library
  return jwt;
}

char* get_books(char*cookie, char* jwt) {
  int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
  char* message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
           "/api/v1/tema/library/books", NULL, cookie, 0, jwt);
  send_to_server(sockfd, message);
  char* response = receive_from_server(sockfd);
  close_connection(sockfd);
  if (basic_extract_json_response(response) == NULL) {
    printf("Nu sunt carti in biblioteca\n");
  } else {
    printf("%s\n", basic_extract_json_response(response));
  }
  return response;
}

char* add_book(char* cookie, char* jwt) {
  char title[50];
  char author[50];
  char genre[50];
  char publisher[50];
  int page_count;
  printf("title=");
  fgets(title, 50, stdin);
  title[strcspn(title, "\n")] = 0;

  printf("author=");
  fgets(author, 50, stdin);
  author[strcspn(author, "\n")] = 0;

  printf("genre=");
  fgets(genre, 50, stdin);
  genre[strcspn(genre, "\n")] = 0;

  printf("publisher=");
  fgets(publisher, 50, stdin);
  publisher[strcspn(publisher, "\n")] = 0;

  printf("page_count=");
  fscanf(stdin, "%d", &page_count);
  if (page_count < 0) {
    printf("Eroare page_count. Revenim in promptul principal\n");
    return NULL;
  }

  JSON_Value *root_value = json_value_init_object();
  JSON_Object *root_object = json_value_get_object(root_value);
  char *serialized_string = NULL;
  json_object_set_string(root_object, "title", title);
  json_object_set_string(root_object, "author", author);
  json_object_set_string(root_object, "genre", genre);
  json_object_set_number(root_object, "page_count", page_count);
  json_object_set_string(root_object, "publisher", publisher);
  serialized_string = json_serialize_to_string_pretty(root_value);

  int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
  char* message = compute_post_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                          "/api/v1/tema/library/books", serialized_string, cookie, 0, jwt);
  send_to_server(sockfd, message);
  char* response = receive_from_server(sockfd);
  printf("Cartea a fost adaugata cu succes\n");
  close_connection(sockfd);
  return response;
}

char* get_book(char* cookie, char* jwt) {
  // read book id
  int id;
  printf("id=");
  fscanf(stdin, "%d", &id);

  int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
  char path[100];
  sprintf(path, "/api/v1/tema/library/books/%d", id);
  char* message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                           path, NULL, cookie, 0, jwt);
  send_to_server(sockfd, message);
  char* response = receive_from_server(sockfd);
  close_connection(sockfd);
  if (basic_extract_json_response(response) == NULL) {
    printf("Autentificarea s-a realizat cu succes\n");
  } else {
    printf("%s\n", basic_extract_json_response(response));
  }
  return response;
}

char* delete_book(char* cookie, char* jwt) {
  // read book id
  int id;
  printf("id=");
  fscanf(stdin, "%d", &id);

  int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
  char path[100];
  sprintf(path, "/api/v1/tema/library/books/%d", id);
  char* message = compute_delete_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                            path, cookie, 0, jwt);
  send_to_server(sockfd, message);
  char* response = receive_from_server(sockfd);
  close_connection(sockfd);
  printf("Cartea a fost stearsa din biblioteca\n");
  return response;
}

char* logout(char* cookie) {
  int sockfd = open_connection("3.8.116.10", 8080, AF_INET, SOCK_STREAM, 0);
  char* message = compute_get_request("ec2-3-8-116-10.eu-west-2.compute.amazonaws.com",
                                 "/api/v1/tema/auth/logout", NULL, cookie, 0, NULL);
  send_to_server(sockfd, message);
  char* response = receive_from_server(sockfd);
  close_connection(sockfd);
  printf("%s\n", response);
  return response;
}

char* createJSON(char* keys[50], char* values[50], int size){
  JSON_Value *root_value = json_value_init_object();
  JSON_Object *root_object = json_value_get_object(root_value);
  char *serialized_string = NULL;
  for (int i = 0; i < size; i++) {
    json_object_set_string(root_object, keys[i], values[i]);
  }
  serialized_string = json_serialize_to_string_pretty(root_value);
  return serialized_string;
}
