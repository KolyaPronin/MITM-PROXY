#include "headerFile.h"
void proxy_ssl_to_ssl(SSL *ssl_client, SSL *ssl_remote) {
    char buf[8192];
    int n;
    // FILE *file_c2s = fopen("clientToServer.txt", "w");
    // FILE *file_s2c = fopen("serverToClient.txt", "w");

    // if (!file_c2s || !file_s2c) {
    //     perror("fopen");
    //     return;
    // }

    int fd_client = SSL_get_fd(ssl_client);
    int fd_remote = SSL_get_fd(ssl_remote);

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fd_client, &readfds);
        FD_SET(fd_remote, &readfds);

        int maxfd = fd_client > fd_remote ? fd_client : fd_remote;
        if (select(maxfd + 1, &readfds, NULL, NULL, NULL) <= 0)
            break;

        if (FD_ISSET(fd_client, &readfds)) {
            n = SSL_read(ssl_client, buf, sizeof(buf));
            if (n <= 0) break;
            SSL_write(ssl_remote, buf, n);
            // fwrite(buf, 1, n, file_c2s);  
        }

        if (FD_ISSET(fd_remote, &readfds)) {
            n = SSL_read(ssl_remote, buf, sizeof(buf));
            if (n <= 0) break;
            SSL_write(ssl_client, buf, n);
            // fwrite(buf, 1, n, file_s2c);  
        }
    }

    // fclose(file_c2s);
    // fclose(file_s2c);
}

