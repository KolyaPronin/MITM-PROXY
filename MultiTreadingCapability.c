#include "headerFile.h"

void *multiThreadingCapability(void *arg) {

    int client_socket = *((int*)arg);
    free(arg);


    http_args args_from_parsing_funk = parsingHttpRequest(client_socket); // парсим http запрос, вытаскиваем url и host

    if(connect_method_handling(args_from_parsing_funk, client_socket)){ // обработка https 
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
