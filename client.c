#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "parson.h"
#include "helpers.h"
#include "requests.h"

int main(int argc, char *argv[])
{
    // save cookie-ul and JWT token
    char* cookie = NULL;
    char* jwt = NULL;
    while(1) {
      // read stdin command
      char command[20];
      fgets(command, 20, stdin);

      // receive 'exit' command
      if (strncmp(command, "exit", 4) == 0) {
        break;
      } else if (strncmp(command, "register", 8) == 0) {

        char username_register[50];
        char password_register[50];
        // read username and password
        printf("username=");
        fgets(username_register, 50, stdin);
        username_register[strcspn(username_register, "\n")] = 0;

        printf("password=");
        fgets(password_register, 50, stdin);
        password_register[strcspn(password_register, "\n")] = 0;

        register_command(username_register, password_register);

      } else if (strncmp(command, "login", 5) == 0) {
        // in case someone is altready authenticated
        if (cookie != NULL) {
          printf("Autentificare esuata\n");
        } else {
          char login_username[50];
          char login_password[50];
          printf("username=");
          fgets(login_username, 50, stdin);
          login_username[strcspn(login_username, "\n")] = 0;

          printf("password=");
          fgets(login_password, 50, stdin);
          login_password[strcspn(login_password, "\n")] = 0;

          // generate cookie for login session
          cookie = login_command(login_username, login_password);
        }

      } else if (strncmp(command, "enter_library", 13) == 0) {
        // if cookie is not active then authentication was not done
        if(cookie == NULL) {
          printf("Nu sunteti autentificat\n");
        } else {
          // generate jwt token
          jwt = enter_library(cookie);
        }

      } else if (strncmp(command, "get_books", 9) == 0) {
        // if jwt is not active, then there is no access key to library
        if (jwt == NULL) {
          printf("Nu aveti acces la biblioteca\n");
        } else {
         get_books(cookie, jwt);
        }

      } else if (strcmp(command, "get_book\n") == 0) {
        if (jwt == NULL) {
          printf("Nu aveti acces la biblioteca\n");
        } else {
          get_book(cookie, jwt);
        }
      } else if (strncmp(command, "add_book", 8) == 0) {
        if (jwt == NULL) {
          printf("Nu aveti acces la biblioteca\n");
        } else {
          add_book(cookie, jwt);
        }
      } else if (strncmp(command, "delete_book", 11) == 0) {
        if (jwt == NULL) {
          printf("Nu aveti acces la biblioteca\n");
        } else {
          delete_book(cookie, jwt);
        }
      } else if (strncmp(command, "logout", 6) == 0) {
        if (cookie == NULL) {
          printf("Nu sunteti autentificat\n");
        } else {
          logout(cookie);
          // the cookie and the JWT token become inactive
          cookie = NULL;
          jwt = NULL;
        }
      } else {

      }

    }

    return 0;
}
