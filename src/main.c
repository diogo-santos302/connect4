#include "server_socket.h"
#include <stdio.h>

int main(void) {
    printf("Starting server...");
    server_socket_init();
    int client_fd = server_socket_accept();
    printf("Client connected");
    char* message = "Hello world";
    server_socket_send(client_fd, message);
    server_socket_close();
    return 0;
}
