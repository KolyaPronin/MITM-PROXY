#include "headerFile.h"
void get_answer_from_server(http_args args_from_parsing_funk, int remote_socket, int client_socket){
    while ((args_from_parsing_funk.total_read = read(remote_socket, args_from_parsing_funk.buffer, sizeof(args_from_parsing_funk.buffer))) > 0) { 
        ssize_t total_sent = 0;
        while (total_sent < args_from_parsing_funk.total_read) {
            ssize_t sent = write(client_socket, args_from_parsing_funk.buffer + total_sent, args_from_parsing_funk.total_read - total_sent);
            if (sent <= 0) {
                perror("write to client");
                break;
            }
            total_sent += sent;
        }
    }
}
