#include "headerFile.h"
int connect_method_handling(http_args args_from_parsing_funk, int client_socket) {
    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    if (strcmp(args_from_parsing_funk.method, "CONNECT") == 0) {
        char host[256] = {0};
        int port = 443;  

        if (sscanf(args_from_parsing_funk.url, "%255[^:]:%d", host, &port) < 1) {
            fprintf(stderr, "Failed to parse host and port from URL: %s\n", args_from_parsing_funk.url);
            free(buffer);
            return -1;
        }

        int remote_socket = handle_connect(host, port, client_socket);
        if (remote_socket == -1) {
            free(buffer);
            return -1;
        }

        
        while (1) {
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(client_socket, &readfds);
            FD_SET(remote_socket, &readfds);

            int nfds = (client_socket > remote_socket ? client_socket : remote_socket) + 1;
            int activity = select(nfds, &readfds, NULL, NULL, NULL);
            if (activity < 0) {
                if (errno == EINTR) continue;
                perror("select");
                break;
            }

            if (FD_ISSET(client_socket, &readfds)) {
                ssize_t bytes = read(client_socket, buffer, BUFFER_SIZE);
                if (bytes <= 0) break;
                if (write(remote_socket, buffer, bytes) <= 0) break;

                FILE *file = fopen("clientToServer.txt", "w");
                if (file) {
                    fwrite(buffer, 1, bytes, file);
                    fclose(file);
                } else {
                    perror("fopen clientToServer.txt");
                }
            }

            if (FD_ISSET(remote_socket, &readfds)) {
                ssize_t bytes = read(remote_socket, buffer, BUFFER_SIZE);
                if (bytes <= 0) break;
                if (write(client_socket, buffer, bytes) <= 0) break;

                FILE *file = fopen("ServerToClient.txt", "w");
                if (file) {
                    fwrite(buffer, 1, bytes, file);
                    fclose(file);
                } else {
                    perror("fopen ServerToClient.txt");
                }
            }
        }

        close(remote_socket);
        close(client_socket);
        free(buffer);
        return 1;  
    }

    free(buffer);
    return 0;  
}
