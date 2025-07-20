#include "headerFile.h"
void rebuild_http_request(http_args args_from_parsing_funk){
    char *path = strstr(args_from_parsing_funk.url, "://");
    if (path) {
        path += 3; 
        path = strchr(path, '/'); 
        if (!path) path = "/";
    } else {
        path = args_from_parsing_funk.url; 
    }

    char new_request[BUFFER_SIZE];
    snprintf(new_request, sizeof(new_request),
            "%s %s HTTP/1.1\r\n", args_from_parsing_funk.method, path);

    char *headers_start = strstr(args_from_parsing_funk.buffer, "\r\n");
    if (headers_start) {
        strncat(new_request, headers_start + 2, sizeof(new_request) - strlen(new_request) - 1);
    }

    strncpy(args_from_parsing_funk.buffer, new_request, sizeof(args_from_parsing_funk.buffer));
    args_from_parsing_funk.total_read = strlen(args_from_parsing_funk.buffer);
}
