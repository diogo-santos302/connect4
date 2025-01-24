#include "client_socket.h"
#include "game_state.h"
#include <stdio.h>

int main(void) {
    printf("Starting client...");
    client_socket_init();
    char message[BUFFER_SIZE] = {0};
    GameState* game_state;
    do {
        client_socket_read(message);
        game_state = game_state_deserialize(message, BUFFER_SIZE);
    } while (!game_state->game_over);
    client_socket_close();
    return 0;
}
