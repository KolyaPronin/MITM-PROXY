#include "headerFile.h"

void *multiThreadingCapability(void *arg) {

    int client_socket = *((int*)arg);
    free(arg);


    http_args args_from_parsing_funk = parsingHttpRequest(client_socket); // парсим http запрос, вытаскиваем url и host

    // if(connect_method_handling(args_from_parsing_funk, client_socket)){ // обработка https 
    //     return NULL;
    // }
    if (strcmp(args_from_parsing_funk.method, "CONNECT") == 0) {
        char host[256] = {0};
        int port = 443;
        if (sscanf(args_from_parsing_funk.url, "%255[^:]:%d", host, &port) < 1) {
            fprintf(stderr, "Failed to parse host and port from URL: %s\n", args_from_parsing_funk.url);
            close(client_socket);
            return NULL;
        }

        int remote_socket = handle_connect(host, port, client_socket);
        if (remote_socket == -1) {
            close(client_socket);
            return NULL;
        }

        SSL *ssl_client = create_tls_server(args_from_parsing_funk, client_socket, host);
        if (!ssl_client) {
            close(remote_socket);
            close(client_socket);
            return NULL;
        }

        SSL *ssl_remote = create_tls_client(remote_socket);
        if (!ssl_remote) {
            SSL_free(ssl_client);
            close(remote_socket);
            close(client_socket);
            return NULL;
        }

        proxy_ssl_to_ssl(ssl_client, ssl_remote);

        SSL_free(ssl_client);
        SSL_free(ssl_remote);
        close(remote_socket);
        close(client_socket);
        return NULL;
    }

    // все что ниже относится к http запросам.
    rebuild_http_request(args_from_parsing_funk); // пересобираем http запрос, чтобы получить ответ с удаленного сервера

    int remote_socket = socket(AF_INET, SOCK_STREAM, 0); // сокет который нужен для перенаправления запроса.
    if (remote_socket < 0) {
        perror("socket remote");
        close(client_socket);
        return NULL;
    }

    int answer = get_ip_from_remote_server(args_from_parsing_funk, client_socket, remote_socket); // получаем ip из ссылки на удаленный сервер
    if(answer == -1){
        return NULL;
    }
    
    sending_request_to_server(args_from_parsing_funk, remote_socket);   // Отправка HTTP-запроса на сайт
    get_answer_from_server(args_from_parsing_funk,  remote_socket, client_socket); // Получение ответа от сайта и пересылка клиенту 

    free(args_from_parsing_funk.buffer);
    close(remote_socket);
    close(client_socket);
    printf("Thread exiting...\n");
    return NULL;
}
