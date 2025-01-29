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
        printf("Column: ");
        column = getchar();
        while (getchar() != '\n');
        if (column < 1 + '0' || column > BOARD_COLS + '0') {
            printf("Invalid column (%d)\n", column);
            continue;
        }
        char column_message = (char) (column - 1);
        printf("Message: %c\n", column_message);
        int bytes_sent = client_socket_send(&column_message, sizeof(char));
        printf("Bytes sent: %d\n", bytes_sent);
        if (bytes_sent < 0) {
            perror("Internal error. Terminating...");
            break;
        }
        int bytes_received = client_socket_read(message);
        if (bytes_received < 0) {
            perror("Internal error. Terminating...");
            break;
        }
        game_state = game_state_deserialize(message, BUFFER_SIZE);
        game_state_print(game_state);
    } 
    client_socket_close();
    return 0;
}
