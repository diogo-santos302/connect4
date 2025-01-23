#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 4444
#define BUFFER_SIZE 1024

extern int socket_fd;

void client_socket_init(void);

void client_socket_read(char* message);

void client_socket_send(char* message);

void client_socket_close(void);

#endif // CLIENT_SOCKET_H
