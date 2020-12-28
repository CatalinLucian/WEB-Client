    WEB Client. REST API communication

* Introduction

In the modern web context, the most used protocol is HTTP
(hypertext transfer protocol). The projects simulates the interaction between a
client and a web server where the client sends different http requests which are
then processed by the server and the result is sent back to the client.
The project follows the client-server paradigm.

* Functionality

- The server is preimplemented. It exposes a REST API. Can be thought of as
a black box which can interact with the user by taking his http requests and
processing them. In this context the server represents a virtual web library.
The server is represented by the host:
ec2-3-8-116-10.eu-west-2.compute.amazonaws.com:8080
- The client is a C program which accepts stdin command (representing http reqs)
and sends them to the server. His role is to function as a user interface for
the virtual library.

 *Register*
To have access to the virtual library the client has to have an account. So
the first step is to *register*. He does this by sending a POST request to
server with his username and password. If the username is already in use, the
server refuses registration

 *Authentication*
  The client sends a POST request with his username and password. If the request
  succeeds then in the server response a session-cookie will be found. The
  server sends back an error message if the credentials are wrong.

 *Access request to library*
  The client sends a GET request. He proves that he is authenticated by using
  the cookie received in the authentication request. The server sends back a
  JWT token which proves the client's access to library. If client is not
  authenticated, server sends an error message.

  *Find info about all books*
  The client sends a GET request to the server. The server responds with info
  about all books which are currently in the library. If the client does not
  prove his access to library, he receives an error message.

  *Find info about book by ID*
  The client sends a GET request to the server. The server responds with info
  about one specific book wanted by client. The info received contains title,
  id, author, publisher, genre and page count of book. If the client does not
  prove his access to library, he receives an error message. If the id is wrong,
  then client receives an error.

  *Add book to library*
  Client sends a POST request containing all info of a newly added book. The
  client has to prove his access to the library.

  *Delete book from library*
  Client sends a DELETE request telling the server to delete all info about the
  book with the given id, The id should be valid and the client should prove his
  access to the virtual library.

  *Logout*
  Client requests a logout to the server by sending a GET request. His session
  cookie and access token are being made invalid.

  All info about the books is being sent/received in JSON format.
