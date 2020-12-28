#ifndef _HELPERS_
#define _HELPERS_

#define BUFLEN 4096
#define LINELEN 1000

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_to_server(int sockfd, char *message);

// receives and returns the message from a server
char* receive_from_server(int sockfd);

// extracts and returns a JSON from a server response
char* basic_extract_json_response(char *str);

// extracts cookie from http response
// if there is no cookie it returns NULL
char* get_cookie(char* response);

// creates and sends register command
// returns the response from server
char* register_command(char* username, char* password);

// creates and sends the login_command
// returns the seesion cookie created
char* login_command(char* username, char* password);

// creates and sends the enter_library command
// returns the JWT token created
char* enter_library(char* cookie);

// creates and sends the get_books command
// prints list of books
// returns the response from server
char* get_books(char* cookie, char* jwt);

// creates and sends the add_book command with the prompt being introduced
// from stdin
// returns the response from the server
char* add_book(char* cookie, char* jwt);

// creates and sends the get_book command
// the id is read into the function
// returns the response from the server
char* get_book(char* cookie, char* jwt);

// creates and sends the delete_book command
// the id is passed as parameter
// returns the response from the server
char* delete_book(char* cookie, char* jwt);

// logs-out from the server
char* logout(char* cookie);

// create JSON body given the keys and the values
char* createJSON(char* keys[50], char* values[50], int size);


#endif
