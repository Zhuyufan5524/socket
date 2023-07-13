#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#define SIZE 0x2000

long int process(int, const char*, char*);
int setNonBlocking(int);

int main(int argc, char** argv) {
    int fd = 0;
    int server_fd = 0;
    unsigned short int port = 80;
    struct sockaddr_in addr = {0}, server_addr = {0};
    char* url = NULL;
    char* data = NULL;
    char* buffer = NULL;
            SSL_CTX* _ssl_ctx = NULL;
        SSL* _ssl = NULL;


    // if (argc >= 4) {
    //     url = argv[1];
    //     port = atoi(argv[2]);
    // } else
    //     exit(EXIT_FAILURE);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("Create server socket file descriptor fail");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    buffer = (char*) malloc(sizeof(char) * SIZE);
    data = (char*) malloc(sizeof(char) * SIZE);
    memset(&server_addr, '\0', sizeof(struct sockaddr));
    memset(buffer, '\0', SIZE);
    memset(data, '\0', SIZE);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_fd, (const struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0) {
        perror("Bind 0.0.0.0:8080 error");
        free(buffer);
        free(data);
        exit(EXIT_FAILURE);
        close(server_fd);
    }
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        free(buffer);
        free(data);
        exit(EXIT_FAILURE);
        close(server_fd);
    }
    for (int i = 0; i < 3; i++) {
        struct sockaddr_in client_addr = {0};
        socklen_t len = sizeof(struct sockaddr);
        int client_fd = 0;
        char* url = (char*) malloc(sizeof(char) * SIZE);
        char* _buffer = (char*) malloc(sizeof(char) * SIZE);
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_CTX* ssl_ctx = NULL;
        SSL* ssl = NULL;
        _ssl_ctx = SSL_CTX_new(TLS_server_method());
        SSL_CTX_set_verify(_ssl_ctx, SSL_VERIFY_PEER, NULL);
        SSL_CTX_set_default_verify_paths(_ssl_ctx);


        memset(&client_addr, '\0', sizeof(struct sockaddr));
        memset(data, '\0', SIZE);
        memset(url, '\0', SIZE);

        if ((client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &len)) < 0) {
            perror("Accept user");
            continue;
        }
        printf("Accept %s:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        for (int i = 0, total = 0; (i = read(client_fd, buffer + total, SIZE - total)); total += i > 0 ? i : 0) {
            if (i == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
                sleep(1);
            else
                break;
        }
        // printf("First reponse: \n%s\n", buffer);
        memcpy(_buffer, buffer, SIZE);
        char* s = strchr(_buffer, ' ') + 1;
        *(s + (strchr(s + 1, ' ') - s)) = '\0';
        if (strstr(s, "http://")) {
            s += 7;
            char* p = NULL;
            if ((p = strchr(s, '/'))) {
                strncpy(url, s, p - s);
                strcat(url, ":80");
                strcat(url, p);
            } else {
                strncpy(url, s, strlen(s));
                strcat(url, ":80");
            }
        } else {
            strcpy(url, s);
        }
        printf("S1: %s\n\n\n", url);
        strcpy(data, "HTTP/1.1 200 Connection Established\r\n\r\n");
        printf("Sended: %zd bytes\n", write(client_fd, data, strlen(data)));
        // if (!(ssl_ctx = SSL_CTX_new(TLS_client_method())))
        //     fprintf(stderr, "%s\n", "Create OPENSSL context failure.");
        // if (!(ssl = SSL_new(ssl_ctx)))
        //     fprintf(stderr, "%s\n", "Create OPENSSL failure.");

        // if (!(_ssl_ctx = SSL_CTX_new(TLSv1_2_server_method())))
        //     fprintf(stderr, "%s\n", "Create OPENSSL context failure.");
        // SSL_CTX_set_options(_ssl_ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
        // 创建 SSL 上下文
        // SSL_CTX_set_cipher_list(_ssl_ctx, "ALL:!aNULL:!eNULL");
// if (_ssl_ctx == NULL) {
//     ERR_print_errors_fp(stderr);
//     return -1;
// }

// // 设置支持的协议版本为 TLSv1.3
// if (SSL_CTX_set_min_proto_version(_ssl_ctx, TLS1_3_VERSION) != 1) {
//     ERR_print_errors_fp(stderr);
//     SSL_CTX_free(_ssl_ctx);
//     return -1;
// }

        // SSL_CTX_load_verify_locations(_ssl_ctx, NULL, "/etc/ssl/certs");
        if (!(_ssl = SSL_new(_ssl_ctx)))
            fprintf(stderr, "%s\n", "Create OPENSSL failure.");

        SSL_set_fd(_ssl, client_fd);
        printf("Accept return: %d\n", SSL_accept(_ssl));
        //     fprintf(stderr, "%s\n", "1: SSL connect to address failure.");
        //     perror("ssl connect");
        // }
        printf("Sended: %d\n", SSL_write(_ssl, "HTTP/1.1 200 Connection Established\r\n\r\n", 40));

        
        // if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        //     perror("Create socket for zl");
        //     close(client_fd);
        //     free(url);
        //     free(_buffer);
        //     close(fd);
        //     break;
        // };

        // addr.sin_family = AF_INET;
        // addr.sin_port = htons(443);
        // inet_pton(AF_INET, "153.3.236.22", &(addr.sin_addr.s_addr));
        // if ((connect(fd, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
        //     perror("Connect zl");
        //     close(client_fd);
        //     free(url);
        //     free(_buffer);
        //     close(fd);
        //     break;
        // }
        // // printf("Return: %d\n", setNonBlocking(fd));
        // memset(data, '\0', SIZE);
        // memset(_buffer, '\0', SIZE);
        // snprintf(data, SIZE, "CONNECT %sHTTP/1.1\r\nHost: 153.3.236.22\r\nUser-Agent: baiduboxapp\r\nX-T5-Auth: 683556443\r\nProxy-Connection: Keep-Alive\r\nConnection: Keep-Alive\r\n\r\n", url);
        // puts(data);
        // for (int i = 0, total = 0; (i = write(fd, data, strlen(data))); total += i > 0 ? i : 0) {
        //     if (i == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        //         sleep(1);
        //     else
        //         break;
        // }
        // for (int i = 0, total = 0; (i = read(fd, _buffer, SIZE)); total += i > 0 ? i : 0) {
        //     if (i == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        //         sleep(1);
        //     else
        //         break;
        // }
        // // puts(_buffer);
        // if (!strstr(buffer, "CONNECT")) {
            // for (int i = 0, total = 0; (i = write(client_fd, "HTTP/1.1 200 Connection Established\r\n\r\n", 40)); total += i > 0 ? i : 0) {
            //     if (i == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            //         printf("%d ", i);
            //     else
            //         break;
            // }
            // printf("Sended: %d\n", );
            // puts("?");
            // memset(buffer, '\0', SIZE);
            // for (int i = 0, total = 0; (i = SSL_read(_ssl, buffer + total, SIZE - total)); total += i > 0 ? i : 0) {
            //     if (i == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            //         printf("%d ", total);
            //     else
            //         break;
            // }
           
            // // printf("Received %d bytes\n", SSL_read(_ssl, buffer, SIZE));
            // puts(buffer);


            

// int result = SSL_write(_ssl, _buffer, strlen(_buffer));
// if (result <= 0) {
//     int error = SSL_get_error(ssl, result);
//     switch (error) {
//         case SSL_ERROR_WANT_READ:
//             puts("需要更多读操作才能继续");
//             break;
//         case SSL_ERROR_WANT_WRITE:
//             puts("需要更多写操作才能继续");
//             break;
//         case SSL_ERROR_ZERO_RETURN:
//             puts("对方已关闭连接");
//             break;
//         case SSL_ERROR_SYSCALL:
//             // 发生系统调用错误
//             perror("System call error");
//             break;
//         case SSL_ERROR_SSL:
//             // 发生 SSL/TLS 错误
//             puts("THIS");
//             ERR_print_errors_fp(stderr);
//             break;
//         default:
//             puts("其他错误");
//             break;
//     }
// }
            // SSL_get_error(ssl, -1);
            // perror("send");
            

            // printf("Received %d bytes\n", SSL_read(_ssl, buffer, SIZE));

            // printf("Received %d bytes\n", SSL_read(_ssl, buffer, SIZE));

            // printf("Received %d bytes\n", SSL_read(_ssl, buffer, SIZE));

            // printf("Received %d bytes\n", SSL_read(_ssl, buffer, SIZE));

            // printf("Received %d bytes\n", SSL_read(_ssl, buffer, SIZE));

            // printf("Received %d bytes\n", SSL_read(_ssl, buffer, SIZE));

            // for (int i = 0, total = 0; (i = read(client_fd, buffer, SIZE)); total += i > 0 ? i : 0) {
            //     if (i == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            //         printf("%d ", total);
            //     else
            //         break;
            // }
            // // memset(buffer, '\0', SIZE);
            // for (int i = 0, total = 0; (i = read(client_fd, buffer, SIZE)); total += i > 0 ? i : 0) {
            //     if (i == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            //         printf("%d ", total);
            //     else
            //         break;
            // }

        // }
        // memset(_buffer, '\0', SIZE);
        // SSL_set_fd(ssl, fd);
        // if (SSL_connect(ssl) < 0) {
        //     fprintf(stderr, "%s\n", "2: SSL connect to address failure.");
        //     perror("ssl connect");
        // }
        
        // puts("Success");
        // puts(buffer);
        // memset(buffer, '\0', SIZE);
        // strcpy(buffer, "GET /get HTTP/1.1\r\nHost: httpbin.org\r\nUser-Agent: C/TESTER\r\n\r\n");
        // for (int i = 0, total = 0; (i = SSL_write(ssl, buffer, strlen(buffer))); total += i > 0 ? i : 0) {
        //     if (i == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        //         sleep(1);
        //     else
        //         break;
        // }
        // for (int i = 0, total = 0; (i = SSL_read(ssl, _buffer + total, SIZE - total)); total += i > 0 ? i : 0) {
        //     if (i == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        //         sleep(1);
        //     else
        //         break;
        // }
        // puts(_buffer);
        // for (int i = 0, total = 0; (i = write(client_fd, _buffer, strlen(_buffer))); total += i > 0 ? i : 0) {
        //     if (i == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        //         sleep(1);
        //     else
        //         break;
        // }
        // SSL_shutdown(ssl);
        // SSL_free(ssl);
        // SSL_CTX_free(ssl_ctx);

        SSL_shutdown(_ssl);
        SSL_free(_ssl);
        SSL_CTX_free(_ssl_ctx);
        close(client_fd);
        free(url);
        free(_buffer);
        // close(fd);
    }

    // snprintf(data, SIZE, "CONNECT %s:%uHTTP/1.1\r\nHost: 153.3.236.22\r\nUser-Agent: baiduboxapp\r\nX-T5-Auth: 683556443\r\nProxy-Connection: Keep-Alive\r\nConnection: Keep-Alive\r\n\r\n", url, port);
    // fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // printf("Create socket file object: %d\n", fd);
    // addr.sin_family = AF_INET;
    // addr.sin_port = htons(443);
    // printf("%d\n", inet_pton(AF_INET, "153.3.236.22", &(addr.sin_addr.s_addr)));

    // printf("Connect return: %d\n", connect(fd, (struct sockaddr *)&addr, sizeof(addr)));
    // printf("Return: %d\n", setNonBlocking(fd));
    // printf("Received: %ld\n", process(fd, data, buffer));
    // puts(buffer);
    // memset(data, '\0', SIZE);
    // memset(buffer, '\0', SIZE);
    // for (int i = 3; i < argc; i++) {
    //     strcat(data, argv[i]);
    //     strcat(data, (i == argc - 1) ? "\r\n\r\n" : "\r\n");
    // }
    // printf("Received: %ld\n", process(fd, data, buffer));
    // puts(buffer);
    // memset(data, '\0', SIZE);
    // memset(buffer, '\0', SIZE);
    // strcpy(data, "GET /get?a=0 HTTP/1.1\r\nHost: 54.165.93.75\r\nConnection: Keep-Alive\r\n\r\n");
    // printf("Received: %ld\n", process(fd, data, buffer));
    // puts(buffer);
    // memset(data, '\0', SIZE);
    // memset(buffer, '\0', SIZE);
    // strcpy(data, "POST /post HTTP/1.1\r\nHost: 54.165.93.75\r\nAccept: application/json\r\nConnection: Close\r\n\r\n{\"a\":0}");
    // printf("Received: %ld\n", process(fd, data, buffer));
    // puts(buffer);
    // close(fd);
    free(buffer);
    free(data);
    puts("Done.");

    return 0;
}

long int process(int fd, const char* msg, char* buffer) {
    // puts(msg);
    long int total_recv = 0;
    long int total_send = 0;
    for (
        int i = 0;
        (i = write(fd, msg, strlen(msg)));
        total_send += i > 0 ? i : 0
    ) {
        if (i == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
            sleep(1);
        else
            break;
    }
    for (
        int i = 0;
        (i = read(fd, buffer + total_recv, SIZE - total_recv));
        total_recv += i > 0 ? i : 0
    ) {
        if (i == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            sleep(1);
        } else
            break;
    }
    *(buffer + total_recv - 1) = '\0';
    printf("Received: %ld\n", total_recv);
    return total_recv;
}

int setNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return -1;
    }

    flags |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        perror("fcntl");
        return -1;
    }

    return 0;
}
