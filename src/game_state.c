#include "game_state.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

static GameState* _game_state_init(int game_over, int current_player) {
    GameState* game_state = (GameState*) malloc(sizeof(GameState));
    if (game_state == NULL) {
        perror("Failed to allocate memory for GameState");
        return NULL;
    }
    game_state->game_over = game_over;
    game_state->current_player_index = current_player;
    return game_state;
}

GameState* game_state_init(void) {
    GameState* game_state_without_board = _game_state_init(0, 0);
    if (game_state_without_board == NULL) {
        return NULL;
    }
    memset(game_state_without_board->board, EMPTY, sizeof(game_state_without_board->board));
    for (int i = 0; i < MAX_PLAYERS; i++) {
        game_state_without_board->players[i].socket_fd = -1;
        game_state_without_board->players[i].marker = i == 0 ? PLAYER1 : PLAYER2;
    }
    return game_state_without_board;
}

int _check_winner_row(GameState* game_state, int row, int column) {
    char symbol = game_state->current_player_index == 0 ? PLAYER1 : PLAYER2;
    if (BOARD_COLS - column < SEQUENCE) return 0;
    return (game_state->board[row][column + 1] == symbol && 
        game_state->board[row][column + 2] == symbol &&
        game_state->board[row][column + 3] == symbol);
}

int _check_winner_column(GameState* game_state, int row, int column) {
    char symbol = game_state->current_player_index == 0 ? PLAYER1 : PLAYER2;
    if (BOARD_ROWS - row >= SEQUENCE) return 0;
    return game_state->board[row - 1][column] == symbol && 
        game_state->board[row - 2][column] == symbol &&
        game_state->board[row - 3][column] == symbol;
}

int _check_winner_diagonal_right(GameState* game_state, int row, int column) {
    char symbol = game_state->current_player_index == 0 ? PLAYER1 : PLAYER2;
    if (BOARD_ROWS - row < SEQUENCE || BOARD_COLS - column < SEQUENCE) return 0;
    return game_state->board[row - 1][column + 1] == symbol && 
        game_state->board[row - 2][column + 2] == symbol &&
        game_state->board[row - 3][column + 3] == symbol;
}

int _check_winner_diagonal_left(GameState* game_state, int row, int column) {
    char symbol = game_state->current_player_index == 0 ? PLAYER1 : PLAYER2;
    if (BOARD_ROWS - row < SEQUENCE || BOARD_COLS - column >= SEQUENCE) return 0;
    return game_state->board[row - 1][column - 1] == symbol && 
        game_state->board[row - 2][column - 2] == symbol &&
        game_state->board[row - 3][column - 3] == symbol;
}

int _check_winner_diagonal(GameState* game_state, int row, int column) {
    return _check_winner_diagonal_right(game_state, row, column) || _check_winner_diagonal_left(game_state, row, column);
}

int game_state_check_winner(GameState* game_state) {
    char symbol = game_state->current_player_index == 0 ? PLAYER1 : PLAYER2;
    for (int row = BOARD_ROWS - 1; row >= 0; row--) {
        for (int column = 0; column < BOARD_COLS; column++) {
            if (game_state->board[row][column] == symbol) {
                if (_check_winner_row(game_state, row, column)) return 1;
                if (_check_winner_column(game_state, row, column)) return 1;
                if (_check_winner_diagonal(game_state, row, column)) return 1;
            }
        }
    }
    return 0;
}

int game_state_make_move(GameState* game_state, int column) {
    for (int row = BOARD_ROWS - 1; row >= 0; row--) {
        if (game_state->board[row][column] == EMPTY) {
            game_state->board[row][column] = game_state->current_player_index == 0 ? PLAYER1 : PLAYER2;
            return 1;
        }
    }
    return 0;
}

int game_state_serialize(const GameState *game_state, char *buffer, size_t buffer_size) {
    if (game_state == NULL || buffer == NULL) {
        return -1;
    }
    size_t required_size = 0;
    required_size += (BOARD_ROWS * BOARD_COLS);
    required_size += sizeof(int);
    required_size += sizeof(int);
    if (buffer_size < required_size) {
        return -1;
    }
    char *cursor = buffer;
    memcpy(cursor, &game_state->current_player_index, sizeof(int));
    cursor += sizeof(int);
    memcpy(cursor, &game_state->game_over, sizeof(int));
    cursor += sizeof(int);
    for (int i = 0; i < BOARD_ROWS; i++) {
        memcpy(cursor, game_state->board[i], BOARD_COLS);
        cursor += BOARD_COLS;
    }
    return cursor - buffer;
}

GameState* game_state_deserialize(char* buffer, size_t buffer_size) {
    if (buffer == NULL) {
        return NULL;
    }
    size_t required_size = 0;
    required_size += (BOARD_ROWS * BOARD_COLS);
    required_size += sizeof(int);
    required_size += sizeof(int);
    if (buffer_size < required_size) {
        return NULL;
    }
    const char* cursor = buffer;
    int current_player = 0, game_over = 0;
    memcpy(&current_player, cursor, sizeof(int));
    cursor += sizeof(int);
    memcpy(&game_over, cursor, sizeof(int));
    cursor += sizeof(int);
    GameState* game_state = _game_state_init(game_over, current_player);
    if (game_state == NULL) {
        return NULL;
    }
    for (int i = 0; i < BOARD_ROWS; i++) {
        memcpy(game_state->board[i], cursor, BOARD_COLS);
        cursor += BOARD_COLS;
    }
    return game_state;
}

void game_state_close(GameState **game_state) {
    free(*game_state);
    *game_state = NULL;
}
