#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char *cookies, int cookies_count, char* jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    // Add url and query params if needed
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);
    // Add host
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // Add cookies if needed
    if (cookies != NULL) {
      sprintf(line, "Cookie: %s", cookies);
      compute_message(message, line);
    }

    // Add token JWT header if needed
    if (jwt != NULL) {
      sprintf(line, "Authorization: Bearer %s", jwt);
      compute_message(message, line);
    }

    compute_message(message, "\n");
    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char *body_data,
                            char *cookies, int cookies_count, char* jwt)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // Add url to message
     sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // Add host to message
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    strcat(body_data_buffer, body_data);

    // Add content-type and content-length
    sprintf(line, "Content-Type: application/json");
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
    compute_message(message, line);

    // Add cookies and jwt if needed
    if (cookies != NULL) {
      sprintf(line, "Cookie: %s", cookies);
      compute_message(message, line);
    }

    if (jwt != NULL) {
      sprintf(line, "Authorization: Bearer %s", jwt);
      compute_message(message, line);
    }
    compute_message(message, "");

    // Add data to be posted
    compute_message(message, body_data_buffer);


    free(line);
    return message;
}

char* compute_delete_request(char *host, char *url,
                            char *cookies, int cookies_count, char* jwt) {
  char *message = calloc(BUFLEN, sizeof(char));
  char *line = calloc(LINELEN, sizeof(char));

  sprintf(line, "DELETE %s HTTP/1.1", url);
  compute_message(message, line);

  sprintf(line, "Host: %s", host);
  compute_message(message, line);

  if (cookies != NULL) {
    sprintf(line, "Cookie: %s", cookies);
    compute_message(message, line);
  }

  if (jwt != NULL) {
    sprintf(line, "Authorization: Bearer %s", jwt);
    compute_message(message, line);
  }

  compute_message(message, "\n");
  free(line);
  return message;
}
