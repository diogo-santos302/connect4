#include "game_state.h"
#include "server_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    GameState* game_state = game_state_init();
    if (game_state == NULL) {
        fprintf(stderr, "Failed to initialize game state.\n");
        return 1;
    }

    server_socket_init();
    printf("Server waiting for connections...\n");

    int client_fd1 = server_socket_accept();
    if (client_fd1 < 0) {
        fprintf(stderr, "Failed to accept client 1.\n");
        game_state_close(&game_state);
        return 1;
    }
    game_state->players[0].socket_fd = client_fd1;
    game_state->players[0].marker = PLAYER1;
    printf("Client 1 connected\n");

    int client_fd2 = server_socket_accept();
    if (client_fd2 < 0) {
        fprintf(stderr, "Failed to accept client 2.\n");
        game_state_close(&game_state);
        return 2;
    }
    game_state->players[1].socket_fd = client_fd2;
    game_state->players[1].marker = PLAYER2;
    printf("Client 2 connected\n");

    char message[BUFFER_SIZE] = {0};
    int bytes_written, bytes_sent, bytes_received, column;
    while (!game_state->game_over) {
        bytes_written = game_state_serialize(game_state, message, BUFFER_SIZE);
        bytes_sent = server_socket_send(game_state->players[game_state->current_player_index].socket_fd, message, bytes_written);
        if (bytes_sent < 0) {
            perror("Client disconnected!\n");
            break;
        }
        bytes_received = server_socket_read(game_state->players[game_state->current_player_index].socket_fd, message);
        if (bytes_received < 0) {
            perror("Internal error. Terminating...\n");
            break;
        }
        column = message[0] - '0';
        if (!game_state_make_move(game_state, column)) continue;
        printf("Client %d's turn:\n", game_state->current_player_index + 1);
        game_state_print(game_state);
        if (game_state_check_winner(game_state)) {
            game_state->game_over = 1;
            printf("Client %d wins!\n", game_state->current_player_index + 1);
            break;
        }
        game_state->current_player_index = (game_state->current_player_index + 1) % MAX_PLAYERS;
    }
    bytes_written = game_state_serialize(game_state, message, BUFFER_SIZE);
    server_socket_send(client_fd1, message, bytes_written);
    server_socket_send(client_fd2, message, bytes_written);

    server_socket_close();
    game_state_close(&game_state);

    return 0;
}
