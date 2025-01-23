#ifndef SERVER_H
#define SERVER_H

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

GameState game_state;

void init_game(void);

void* handle_client_connection(void* arg);

int check_winner(void);

int make_move(int column);

void send_game_state(void);

void close_game(void);

#endif // SERVER_H
