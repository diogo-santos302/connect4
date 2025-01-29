#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

#include "stdio.h"

#define PORT                4444
#define MAX_CONNECTIONS     2
#define BUFFER_SIZE         1024

extern int socket_fd;

void server_socket_init(void);

int server_socket_accept(void);

int server_socket_read(int client_fd, char* message);

int server_socket_send(int client_fd, char* message, ssize_t message_size);

void server_socket_close(void);

void server_socket_close_client_socket(int client_fd);

#endif // SERVER_SOCKET_H
