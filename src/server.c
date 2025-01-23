#include "server.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

void init_game(void) {
    game_state = (GameState*) malloc(sizeof(GameState));
    if (game_state == NULL) {
        perror("Failed to allocate memory for GameState");
        return;
    }
    memset(game_state->board, EMPTY, sizeof(game_state->board));
    for (int i = 0; i < MAX_PLAYERS; i++) {
        game_state->players[i].socket_fd = -1;
        game_state->players[i].marker = i == 0 ? PLAYER1 : PLAYER2;
    }
    game_state->game_over = 0;
    game_state->current_player_index = 0;
}

int _check_winner_row(int row, int column) {
    char symbol = game_state->current_player_index == 0 ? PLAYER1 : PLAYER2;
    if (BOARD_COLS - column < SEQUENCE) return 0;
    return (game_state->board[row][column + 1] == symbol && 
        game_state->board[row][column + 2] == symbol &&
        game_state->board[row][column + 3] == symbol);
}

int _check_winner_column(int row, int column) {
    char symbol = game_state->current_player_index == 0 ? PLAYER1 : PLAYER2;
    if (BOARD_ROWS - row >= SEQUENCE) return 0;
    return game_state->board[row - 1][column] == symbol && 
        game_state->board[row - 2][column] == symbol &&
        game_state->board[row - 3][column] == symbol;
}

int _check_winner_diagonal_right(int row, int column) {
    char symbol = game_state->current_player_index == 0 ? PLAYER1 : PLAYER2;
    if (BOARD_ROWS - row < SEQUENCE || BOARD_COLS - column < SEQUENCE) return 0;
    return game_state->board[row - 1][column + 1] == symbol && 
        game_state->board[row - 2][column + 2] == symbol &&
        game_state->board[row - 3][column + 3] == symbol;
}

int _check_winner_diagonal_left(int row, int column) {
    char symbol = game_state->current_player_index == 0 ? PLAYER1 : PLAYER2;
    if (BOARD_ROWS - row < SEQUENCE || BOARD_COLS - column >= SEQUENCE) return 0;
    return game_state->board[row - 1][column - 1] == symbol && 
        game_state->board[row - 2][column - 2] == symbol &&
        game_state->board[row - 3][column - 3] == symbol;
}

int _check_winner_diagonal(int row, int column) {
    return _check_winner_diagonal_right(row, column) || _check_winner_diagonal_left(row, column);
}

int check_winner(void) {
    char symbol = game_state->current_player_index == 0 ? PLAYER1 : PLAYER2;
    for (int row = BOARD_ROWS - 1; row >= 0; row--) {
        for (int column = 0; column < BOARD_COLS; column++) {
            if (game_state->board[row][column] == symbol) {
                if (_check_winner_row(row, column)) return 1;
                if (_check_winner_column(row, column)) return 1;
                if (_check_winner_diagonal(row, column)) return 1;
            }
        }
    }
    return 0;
}

int make_move(int column) {
    for (int row = BOARD_ROWS - 1; row >= 0; row--) {
        if (game_state->board[row][column] == EMPTY) {
            game_state->board[row][column] = game_state->current_player_index == 0 ? PLAYER1 : PLAYER2;
            game_state->current_player_index = (game_state->current_player_index + 1) % MAX_PLAYERS;
            return 1;
        }
    }
    return 0;
}

void* handle_client_connection(void* arg) {
    int client_fd = *(int*)arg;
    free(arg);

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

        if (!make_move(column)) {
            send(client_fd, "Column full. Try again.\n", 24, 0);
            continue;
        }

        if (check_winner()) {
            game_state->game_over = 1;
            send_game_state();
            break;
        }

        send_game_state();
    }

    close(client_fd);
    return NULL;
}

static char* _convert_to_string() {
    char* buffer = (char*) malloc(1024);
    if (!buffer) {
        perror("Failed to allocate memory for game state serialization");
        return NULL;
    }
    memset(buffer, 0, 1024);
    char player_info[50];
    snprintf(player_info, sizeof(player_info), "Current Player: %c\n",
             game_state->current_player_index == 0 ? PLAYER1 : PLAYER2);
    strcat(buffer, player_info);
    strcat(buffer, "Game State:\n");
    for (int row = 0; row < BOARD_ROWS; row++) {
        for (int column = 0; column < BOARD_COLS; column++) {
            switch(game_state->board[row][column]) {
                case EMPTY:
                    strcat(buffer, "| ");
                    break;
                case PLAYER1:
                    strcat(buffer, "|X");
                    break;
                case PLAYER2:
                    strcat(buffer, "|O");
                    break;
                default:
                    break;
            }
        }
        strcat(buffer, "|\n");
    }
    return buffer;
}

void send_game_state(void) {
    char* serialized_game_state = _convert_to_string();
    if (!serialized_game_state) {
        fprintf(stderr, "Failed to serialize game state.\n");
        return;
    }
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (game_state->players[i].socket_fd > 0) {
            if (send(game_state->players[i].socket_fd, serialized_game_state, strlen(serialized_game_state), 0) < 0) {
                perror("Failed to send game state to player");
            }
        }
    }
    free(serialized_game_state);
}

void close_game(void) {
    free(&game_state);
    game_state = NULL;
}
