#include "client_socket.h"
#include <arpa/inet.h>
#include <bits/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int socket_fd;

void client_socket_init(void) {
    const int INTERNET_PROTOCOL = 0;
    socket_fd = socket(AF_INET, SOCK_STREAM, INTERNET_PROTOCOL);
    if (socket_fd < 0) {
        perror("Socket creation failed");
        return;
    }
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("Invalid address");
        return;
    }
    if ((connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address))) < 0) {
        perror("Connection failed");
        return;
    }
}

int client_socket_read(char* message) {
    return read(socket_fd, message, BUFFER_SIZE);
}

int client_socket_send(char* message, size_t message_size) {
    const int NO_FLAGS = 0;
    return send(socket_fd, message, message_size, NO_FLAGS);
}

void client_socket_close(void) {
    close(socket_fd);
}
