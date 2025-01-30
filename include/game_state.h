#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <stddef.h>

#define SEQUENCE 4
#define BOARD_ROWS 6
#define BOARD_COLS 7
#define MAX_PLAYERS 2
#define EMPTY ' '
#define PLAYER1 'X'
#define PLAYER2 'O'

typedef struct {
    int socket_fd;
    char marker;
} Player;

typedef struct {
    char board[BOARD_ROWS][BOARD_COLS];
    Player players[MAX_PLAYERS];
    int current_player_index;
    int game_over;
} GameState;

GameState* game_state_init(void);

int game_state_check_winner(GameState* game_state);

int game_state_make_move(GameState* game_state, int column);

int game_state_is_full(GameState* game_state);

/**
 * Serializes the current game state into a buffer.
 * 
 * @param game_state A pointer to the GameState object to serialize.
 * @param buffer A pointer to the buffer where the serialized data will be stored.
 * @param buffer_size The size of the buffer.
 * @return The number of bytes written to the buffer, or -1 if the buffer is insufficient.
 */
int game_state_serialize(const GameState *game_state, char *buffer, size_t buffer_size);

GameState* game_state_deserialize(char* buffer, size_t buffer_size);

void game_state_print(GameState* game_state);

void game_state_close(GameState** game_state);

#endif // GAME_STATE_H
