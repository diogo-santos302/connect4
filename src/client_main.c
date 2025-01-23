#include "client_socket.h"
#include <stdio.h>

int main(void) {
    printf("Starting client...");
    client_socket_init();
    char message[BUFFER_SIZE] = {0};
    client_socket_read(message);
    printf("%s\n", message);
    client_socket_close();
    return 0;
}
