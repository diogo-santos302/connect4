#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

#define PORT                4444
#define MAX_CONNECTIONS     2
#define BUFFER_SIZE         1024

extern int socket_fd;

void server_socket_init(void);

int server_socket_accept(void);

void server_socket_read(int client_fd, char* message);

void server_socket_send(int client_fd, char* message);

void server_socket_close(void);

#endif // SERVER_SOCKET_H
