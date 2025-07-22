#define BUFFER_SIZE 16384
#define _POSIX_C_SOURCE 200112L 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h> 
#include <pthread.h>
#include <signal.h>
#include <sys/select.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/x509v3.h>
#include <fcntl.h>

typedef struct  {
    char method[8];
    char url[2048];
    char *buffer;
    size_t total_read;
}http_args;

typedef struct {
    X509 *cert;
    EVP_PKEY *pkey;
} fake_cert;

typedef struct {
    char protocol[6];
    char status_code[4];
    char message[1024];
    char access_control_allow_origin[256];
    char data[64];
    char pragma[16];
    char expires[32];
    char cache_control[128];
    char access_control_allow_credentials[8];
    char content_type[16];
    char cross_origin_resource_policy[16];
    char content_security_policy_report_only[1024];
    char cross_origin_opener_policy_report_only[1024];
    char report_to[1024];
    char server[16];
    char content_lenght[16];
}dictionary;


void tcpServer();
void *multiThreadingCapability(void *arg);
http_args parsingHttpRequest(int client_socket);
void rebuild_http_request(http_args args_from_parsing_funk);
int get_ip_from_remote_server(http_args args_from_parsing_funk, int client_socket, int remote_socket);
void sending_request_to_server(http_args args_from_parsing_funk, int remote_socket);
void get_answer_from_server(http_args args_from_parsing_funk, int remote_socket, int client_socket);
int handle_connect(const char *host, int port, int client_socket);
int connect_method_handling(http_args args_from_parsing_funk, int client_socket);
fake_cert generate_fake_cert(char *host);
SSL *create_tls_server(http_args args_from_parsing_funk, int client_socket, char *host);
SSL *create_tls_client(int remote_socket);
void proxy_ssl_to_ssl(SSL *ssl_client, SSL *ssl_remote);