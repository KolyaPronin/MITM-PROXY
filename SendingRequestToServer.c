#include "headerFile.h"
void sending_request_to_server(http_args args_from_parsing_funk, int remote_socket){
    ssize_t total_written = 0;
    while (total_written < args_from_parsing_funk.total_read) { // мы гарантируем что весь запрос дойдет до сайта. Так как часть данных может  просто не дойти.
        ssize_t written = write(remote_socket, args_from_parsing_funk.buffer + total_written, args_from_parsing_funk.total_read - total_written);
        if (written <= 0) {
            perror("write to remote");
            break;
        }
        total_written += written;
    }
}