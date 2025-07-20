
#include "headerFile.h"
int handle_connect(const char *host, int port, int client_socket) {
    struct addrinfo hints = {0}, *res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    char port_str[6];
    snprintf(port_str, sizeof(port_str), "%d", port);

    if (getaddrinfo(host, port_str, &hints, &res) != 0) {
        perror("getaddrinfo");
        return -1;
    }

    int remote_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (remote_socket < 0) {
        perror("socket");
        freeaddrinfo(res);
        return -1;
    }

    if (connect(remote_socket, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        close(remote_socket);
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);

    const char *response = "HTTP/1.1 200 Connection Established\r\n\r\n";
    write(client_socket, response, strlen(response));

    return remote_socket;
}
