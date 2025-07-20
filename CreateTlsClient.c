#include "headerFile.h"
SSL *create_tls_client(int remote_socket) {
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        fprintf(stderr, "Failed to create Client_SSL_CTX\n");
        return NULL;
    }
    SSL *ssl = SSL_new(ctx);
        if (!ssl) {
        fprintf(stderr, "Failed to create SSL structure Client\n");
        SSL_CTX_free(ctx);
        return NULL;
    }
    SSL_set_fd(ssl, remote_socket);
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return NULL;
    }
    return ssl;
}
