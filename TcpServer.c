#include "headerFile.h"

void tcpServer() {

    const int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) { // нужен при перезапуске сервера, где порт был недавно занят, использовать порт по новой
        perror("setsockopt");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address = {0};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;


    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) { // связывает сокет с адресом и портом
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) { // переводит сокет в режим слушания, он принимает входящие соединения, переменная говорит сколько соединений может одновременно быть
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Proxy server listening on :8080...\n");

    while (1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("accept");
            continue;
        }

        int *arg = malloc(sizeof(int));
        *arg = client_socket;

        pthread_t thread;
        pthread_create(&thread, NULL, multiThreadingCapability, arg);
        pthread_detach(thread); 
    }

    close(server_socket);
}

int main() {
    signal(SIGPIPE, SIG_IGN);
    tcpServer();
    return 0;
}
