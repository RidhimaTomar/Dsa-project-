#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "globals.h"

// Include all .c files here
#include "user.c"
#include "notes.c"
#include "reminder.c"
#include "server.c"

int main() {
    int server_fd, client_fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[8192];

    initHashTable();

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Server running on port %d\n", PORT);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        memset(buffer, 0, sizeof(buffer));
        read(client_fd, buffer, sizeof(buffer) - 1);
        handleRequest(client_fd, buffer);
        close(client_fd);
    }

    return 0;
}
