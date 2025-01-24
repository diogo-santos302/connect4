#include "game_state.h"
#include "server_socket.h"
#include "client_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

typedef struct {
    int client_fd;
    GameState* game_state;
} ThreadArgs;

static void* handle_client_connection(void* arg) {
    ThreadArgs* args = (ThreadArgs*)arg;
    int client_fd = args->client_fd;
    GameState* game_state = args->game_state;
    free(args);

    char buffer[256];
    int bytes_received;

    while (!game_state->game_over) {
        if (game_state->players[game_state->current_player_index].socket_fd != client_fd) {
            usleep(100000);
            continue;
        }

        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_received <= 0) {
            fprintf(stderr, "Client disconnected.\n");
            close(client_fd);
            return NULL;
        }

        int column = atoi(buffer);
        if (column < 0 || column >= BOARD_COLS) {
            send(client_fd, "Invalid move. Try again.\n", 25, 0);
            continue;
        }

        if (!game_state_make_move(column)) {
            send(client_fd, "Column full. Try again.\n", 24, 0);
            continue;
        }

        if (game_state_check_winner()) {
            game_state->game_over = 1;
            send_game_state();
            break;
        }

        send_game_state();
    }

    close(client_fd);
    return NULL;
}

int main() {
    GameState* game_state = game_state_init();
    if (game_state == NULL) {
        fprintf(stderr, "Failed to initialize game state.\n");
        return 1;
    }

    server_socket_init();

    int client_fd1 = server_socket_accept();
    printf("Client 1 connected");

    game_state->players[0].socket_fd = client_fd1;
    game_state->players[0].marker = PLAYER1;
    
    int client_fd2 = server_socket_accept();
    printf("Client 2 connected");

    game_state->players[1].socket_fd = client_fd2;
    game_state->players[1].marker = PLAYER2;

    pthread_t thread1, thread2;
    ThreadArgs* args1 = (ThreadArgs*)malloc(sizeof(ThreadArgs));
    if (args1 == NULL) {
        perror("Failed to allocate memory for thread arguments");
        server_socket_close();
        game_state_close(&game_state);
        exit(1);
    }
    args1->client_fd = client_fd1;
    args1->game_state = game_state;
    
    ThreadArgs* args2 = (ThreadArgs*)malloc(sizeof(ThreadArgs));
    if (args2 == NULL) {
        perror("Failed to allocate memory for thread arguments");
        server_socket_close();
        game_state_close(&game_state);
        exit(2);
    }
    args2->client_fd = client_fd2;
    args2->game_state = game_state;

    pthread_create(&thread1, NULL, handle_client_connection, args1);
    pthread_create(&thread2, NULL, handle_client_connection, args2);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    server_socket_close();
    game_state_close(&game_state);

    return 0;
}
