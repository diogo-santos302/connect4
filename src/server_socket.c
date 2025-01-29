#include "server_socket.h"
#include <arpa/inet.h>
#include <bits/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int socket_fd;

void server_socket_init(void) {
    const int INTERNET_PROTOCOL = 0;
    socket_fd = socket(AF_INET, SOCK_STREAM, INTERNET_PROTOCOL);
    if (socket_fd < 0) {
        perror("Socket creation failed");
        return;
    }
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);
    if (bind(socket_fd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        perror("Socket bind failed");
        return;
    }
    if (listen(socket_fd, 2) < 0) {
        perror("Listen failed");
        return;
    }
}

int server_socket_accept(void) {
    if (socket_fd < 0) {
        perror("Socket not initialized");
        return -1;
    }
    struct sockaddr_in client_address;
    socklen_t address_length = sizeof(client_address);
    int client_fd = accept(socket_fd, (struct sockaddr*)&client_address, &address_length);
    if (client_fd < 0) {
        perror("Client connection failed");
        return -1;
    }
    return client_fd;
}

int server_socket_read(int client_fd, char* message) {
    return read(client_fd, message, BUFFER_SIZE);
}

int server_socket_send(int client_fd, char* message, ssize_t message_size) {
    const int NO_FLAGS = 0;
    return send(client_fd, message, message_size, NO_FLAGS);
}

void server_socket_close(void) {
    close(socket_fd);
}

void server_socket_close_client_socket(int client_fd) {
    close(client_fd);
}
