#include "game_state.h"
#include "server_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SWITCH_PLAYER(game_state) ((game_state)->current_player_index = ((game_state)->current_player_index + 1) % MAX_PLAYERS)

static int handle_client_connection(GameState* game_state) {
    static int player_index = 0;
    int client_fd = server_socket_accept();
    if (client_fd < 0) {
        return -1;
    }
    game_state->players[player_index].socket_fd = client_fd;
    game_state->players[player_index].marker = (player_index == 0) ? PLAYER1 : PLAYER2;
    printf("Client %d connected\n", player_index + 1);
    player_index++;
    return client_fd;
}

static int send_game_state(GameState* game_state) {
    char message[BUFFER_SIZE] = {0};
    int bytes_written = game_state_serialize(game_state, message, BUFFER_SIZE);
    return server_socket_send(game_state->players[game_state->current_player_index].socket_fd, message, bytes_written);
}

static int get_move(GameState* game_state) {
    char message[BUFFER_SIZE] = {0};
    int bytes_received = server_socket_read(game_state->players[game_state->current_player_index].socket_fd, message);
    if (bytes_received < 0) {
        return -1;
    }
    return message[0] - '0';
}

static void announce_game_winner(GameState* game_state) {
    char message[BUFFER_SIZE] = {0};
    int bytes_written = game_state_serialize(game_state, message, BUFFER_SIZE);
    server_socket_send(game_state->players[0].socket_fd, message, bytes_written);
    server_socket_send(game_state->players[1].socket_fd, message, bytes_written);
}

int main() {
    GameState* game_state = game_state_init();
    if (game_state == NULL) {
        fprintf(stderr, "Failed to initialize game state.\n");
        return 1;
    }
    server_socket_init();
    printf("Server waiting for connections...\n");

    int client_fd1 = handle_client_connection(game_state);
    if (client_fd1 < 0) {
        fprintf(stderr, "Failed to accept client 1.\n");
        game_state_close(&game_state);
        server_socket_close();
        return 1;
    }

    int client_fd2 = handle_client_connection(game_state);
    if (client_fd2 < 0) {
        fprintf(stderr, "Failed to accept client 2.\n");
        game_state_close(&game_state);
        server_socket_close();
        return 2;
    }

    int column;
    while (!game_state->game_over) {
        if (send_game_state(game_state) < 0) {
            fprintf(stderr, "Client disconnected!\n");
            break;
        }
        column = get_move(game_state);
        if (column == -1) {
            fprintf(stderr, "Internal error. Terminating...\n");
            break;
        }
        if (!game_state_make_move(game_state, column)) continue;
        printf("Client %d's turn:\n", game_state->current_player_index + 1);
        game_state_print(game_state);
        if (game_state_check_winner(game_state)) {
            game_state->game_over = 1;
            printf("Client %d wins!\n", game_state->current_player_index + 1);
            break;
        }
        SWITCH_PLAYER(game_state);
    }

    announce_game_winner(game_state);

    server_socket_close_client_socket(client_fd1);
    server_socket_close_client_socket(client_fd2);
    server_socket_close();
    game_state_close(&game_state);

    return 0;
}
