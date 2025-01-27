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
    printf("You play as %c\n", game_state->current_player_index == 0 ? PLAYER1 : PLAYER2);
    int column;
    while (!game_state->game_over) {
        scanf("Column: %d\n", &column);
        if (column < 1 || column > BOARD_COLS) {
            printf("Invalid column\n");
            continue;
        }
        char column_message = (char)column;
        int bytes_sent = client_socket_send(&column_message, sizeof(char));
        printf("Bytes sent: %d\n", bytes_sent);
        client_socket_read(message);
        game_state = game_state_deserialize(message, BUFFER_SIZE);
        game_state_print(game_state);
    } 
    client_socket_close();
    return 0;
}
