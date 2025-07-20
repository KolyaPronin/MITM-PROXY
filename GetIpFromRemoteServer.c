#include "headerFile.h"
int get_ip_from_remote_server(http_args args_from_parsing_funk, int client_socket, int remote_socket){
    char ipstr[INET_ADDRSTRLEN]; // для IP

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints)); // заполняет нулями, равными количеством sizeof(hints), по адресу &hints.
    hints.ai_family = AF_INET;  // IPv4
    hints.ai_socktype = SOCK_STREAM;


    char host[256] = {0};
    char *host_header = strstr(args_from_parsing_funk.buffer, "Host: ");
    if (host_header) {
        sscanf(host_header, "Host: %255s", host);
        printf("Host: %s\n", host);
    }

    if (getaddrinfo(host, NULL, &hints, &res) != 0) {
        perror("getaddrinfo");
        close(client_socket);
        close(remote_socket);
        return -1; 
    } else {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
        printf("Resolved IP: %s\n", ipstr);
        freeaddrinfo(res);
    }

    struct sockaddr_in remote_addr = {0};
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(80);
    
    if (inet_pton(AF_INET, ipstr, &remote_addr.sin_addr) <= 0) { 
        perror("inet_pton remote");
        close(client_socket);
        close(remote_socket);
        return -1;
    }

        if (connect(remote_socket, (struct sockaddr*)&remote_addr, sizeof(remote_addr)) < 0) { // функция устанавливает соединение по переданным в сокет данным, порт адрес и тд
        perror("connect");
        close(client_socket);
        close(remote_socket);
        return -1;
    }
    return 0;
}
