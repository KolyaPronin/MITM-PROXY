#include "headerFile.h"
http_args parsingHttpRequest(int client_socket){
    http_args args_to_return = {0};

    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    args_to_return.total_read = 0;
    while (1) {
        size_t bytes_read = read(client_socket, buffer + args_to_return.total_read, BUFFER_SIZE - args_to_return.total_read - 1);
        if (bytes_read <= 0) {
            close(client_socket);
            break; 
        }
        args_to_return.total_read += bytes_read;
        buffer[args_to_return.total_read] = '\0';
        
        if (strstr(buffer, "\r\n\r\n")) {
            break;
        }

        if (args_to_return.total_read >= BUFFER_SIZE - 1) {
            fprintf(stderr, "Request too large\n");
            close(client_socket);
            continue;
        }
    }

    if (args_to_return.total_read > 0) {
        buffer[args_to_return.total_read] = '\0';

        if(sscanf(buffer,"%7s %2047s", args_to_return.method, args_to_return.url)){
            printf("Method: %s\n", args_to_return.method);
            printf("Url: %s\n", args_to_return.url);
        }
    }
    FILE *file = fopen("output.txt", "w"); 
    if (file) {
        fwrite(buffer, 1, args_to_return.total_read, file);
        fflush(file);
        fclose(file);
    } else {
        perror("fopen");
    }
    args_to_return.buffer = buffer;
    return args_to_return;
}
