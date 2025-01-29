#include "client_socket.h"
#include "game_state.h"
#include <stdio.h>

int main(void) {
    printf("Starting client...\n");
    client_socket_init();
    char message[BUFFER_SIZE] = {0};
    client_socket_read(message);
    GameState* game_state = game_state_deserialize(message, BUFFER_SIZE);
    game_state_print(game_state);
    int player_index = game_state->current_player_index == 0;
    printf("You play as %c!\n", player_index ? PLAYER1 : PLAYER2);
    int bytes_sent, bytes_received, column;
    char column_message;
    while (!game_state->game_over) {
        printf("Your turn! Column: ");
        column = getchar();
        while (getchar() != '\n');
        if (column < 1 + '0' || column > BOARD_COLS + '0') {
            printf("Invalid column (%d). Try again.\n", column - '0');
            continue;
        }
        column_message = (char) (column - 1);
        bytes_sent = client_socket_send(&column_message, sizeof(char));
        if (bytes_sent < 0) {
            perror("Internal error. Terminating...\n");
            break;
        }
        bytes_received = client_socket_read(message);
        if (bytes_received < 0) {
            perror("Internal error. Terminating...\n");
            break;
        }
        game_state = game_state_deserialize(message, BUFFER_SIZE);
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
