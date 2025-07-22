#include "headerFile.h"

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void proxy_ssl_to_ssl(SSL *ssl_client, SSL *ssl_remote) {
    char buf[8192];
    int n;
    int fd_client = SSL_get_fd(ssl_client);
    int fd_remote = SSL_get_fd(ssl_remote);

    set_nonblocking(fd_client);
    set_nonblocking(fd_remote);

    SSL_set_mode(ssl_client, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER | SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_AUTO_RETRY);
    SSL_set_mode(ssl_remote, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER | SSL_MODE_ENABLE_PARTIAL_WRITE | SSL_MODE_AUTO_RETRY);

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(fd_client, &readfds);
        FD_SET(fd_remote, &readfds);

        int maxfd = fd_client > fd_remote ? fd_client : fd_remote;
        int ret = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (ret < 0) {
            if (errno == EINTR) continue;
            perror("select");
            break;
        }

        // CLIENT → REMOTE
        if (FD_ISSET(fd_client, &readfds)) {
            while (1) {
                n = SSL_read(ssl_client, buf, sizeof(buf));
                if (n > 0) {
                    FILE *log = fopen("clientToServer.txt", "w");
                    if (log) {
                        fwrite(buf, 1, n, log);
                        fclose(log);
                    }

                    int written = 0;
                    while (written < n) {
                        int w = SSL_write(ssl_remote, buf + written, n - written); // передаем данные на сервер
                        if (w > 0) {
                            written += w;
                        } else {
                            int err = SSL_get_error(ssl_remote, w);
                            if (err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ) break;
                            return; // error or closed
                        }
                    }
                } else {
                    int err = SSL_get_error(ssl_client, n);
                    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
                        break;
                    return; 
                }
            }
        }

        // REMOTE → CLIENT
        if (FD_ISSET(fd_remote, &readfds)) {
            while (1) {
                n = SSL_read(ssl_remote, buf, sizeof(buf));
                if (n > 0) {
                    dictionary_from_parsing_https_answer(buf);
                    FILE *log = fopen("serverToClient.txt", "w");
                    if (log) {
                        fwrite(buf, 1, n, log);
                        fclose(log);
                    }

                    int written = 0;
                    while (written < n) {
                        int w = SSL_write(ssl_client, buf + written, n - written);
                        if (w > 0) {
                            written += w;
                        } else {
                            int err = SSL_get_error(ssl_client, w);
                            if (err == SSL_ERROR_WANT_WRITE || err == SSL_ERROR_WANT_READ) break;
                            return; 
                        }
                    }
                } else {
                    int err = SSL_get_error(ssl_remote, n);
                    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
                        break;
                    return; 
                }
            }
        }
    }
}
