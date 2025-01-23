#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int server_fd, client_fd1, client_fd2;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    init_game(&game_state);
    if (!game_state) {
        fprintf(stderr, "Failed to initialize game state.\n");
        return 1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Socket bind failed");
        return 1;
    }

    if (listen(server_fd, 2) < 0) {
        perror("Listen failed");
        return 1;
    }

    printf("Server is listening on port %d...\n", SERVER_PORT);

    client_fd1 = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd1 < 0) {
        perror("Client 1 connection failed");
        return 1;
    }
    printf("Client 1 connected.\n");
    game_state->players[0].socket_fd = client_fd1;
    game_state->players[0].marker = PLAYER1;

    client_fd2 = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
    if (client_fd2 < 0) {
        perror("Client 2 connection failed");
        return 1;
    }
    printf("Client 2 connected.\n");
    game_state->players[1].socket_fd = client_fd2;
    game_state->players[1].marker = PLAYER2;

    pthread_t thread1, thread2;
    int* arg1 = malloc(sizeof(int));
    int* arg2 = malloc(sizeof(int));
    *arg1 = client_fd1;
    *arg2 = client_fd2;

    pthread_create(&thread1, NULL, handle_client_connection, arg1);
    pthread_create(&thread2, NULL, handle_client_connection, arg2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    close_game();
    close(server_fd);

    return 0;
}
