#include "headerFile.h"
SSL *create_tls_server(http_args args_from_parsing_funk, int client_socket, char *host) {


    fake_cert dinamic_fake_cert_generation = generate_fake_cert(host);

    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        fprintf(stderr, "Failed to create SSL_CTX\n");
        return NULL;
    }

    if (SSL_CTX_use_certificate(ctx, dinamic_fake_cert_generation.cert) <= 0) {
        fprintf(stderr, "Failed to use certificate\n");
        SSL_CTX_free(ctx);
        return NULL;
    }

    if (SSL_CTX_use_PrivateKey(ctx, dinamic_fake_cert_generation.pkey) <= 0) {
        fprintf(stderr, "Failed to use private key\n");
        SSL_CTX_free(ctx);
        return NULL;
    }

    SSL *ssl = SSL_new(ctx);
    if (!ssl) {
        fprintf(stderr, "Failed to create SSL structure\n");
        SSL_CTX_free(ctx);
        return NULL;
    }

    SSL_set_fd(ssl, client_socket);

    if (SSL_accept(ssl) < 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return NULL;
    }

    return ssl;


}
