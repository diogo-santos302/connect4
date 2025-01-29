#include "client_socket.h"
#include "game_state.h"
#include <stdio.h>

static int get_column_input(void) {
    int column;
    printf("Your turn! Enter column (1-%d): ", BOARD_COLS);
    if (scanf("%d", &column) != 1) {
        while (getchar() != '\n');
    }
    while (getchar() != '\n');
    return column - 1;
}

static int send_move(int column) {
    char column_message = (char) (column + '0');
    return client_socket_send(&column_message, sizeof(char));
}

static GameState* receive_game_state(void) {
    char message[BUFFER_SIZE] = {0};
    int bytes_received = client_socket_read(message);
    if (bytes_received < 0) {
        return NULL;
    }
    return game_state_deserialize(message, BUFFER_SIZE);
}

int main(void) {
    printf("Starting client...\n");
    client_socket_init();
    GameState* game_state = receive_game_state();
    int player_index = game_state->current_player_index == 0;
    game_state_print(game_state);
    printf("You play as %c!\n", player_index ? PLAYER1 : PLAYER2);
    int column;
    while (!game_state->game_over) {
        column = get_column_input();
        if (column < 0 || column >= BOARD_COLS) {
            printf("Invalid column (%d). Try again.\n", column + 1);
            continue;
        }
        if (send_move(column) < 0) {
            fprintf(stderr, "Internal error. Terminating...\n");
            break;
        }
        game_state = receive_game_state();
        if (game_state == NULL) {
            fprintf(stderr, "Internal error. Terminating...\n");
            break;
        }
        game_state_print(game_state);
    } 
    if (game_state->current_player_index != player_index) {
        printf("You win!\n");
    } else {
        printf("You lose!\n");
    }
    client_socket_close();
    game_state_close(&game_state);
    return 0;
}
