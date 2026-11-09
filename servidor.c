//Juan David Morocho Angulo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO 9999
#define BUFFER_SIZE 1024
#define ARCHIVO "Google.html"

int main() {
    int sockfd, newsockfd;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;

    signal(SIGCHLD, SIG_IGN);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error al abrir el socket del servidor");
        exit(1);
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PUERTO);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error en bind");
        close(sockfd);
        exit(1);
    }

    if (listen(sockfd, 5) < 0) {
        perror("Error en listen");
        close(sockfd);
        exit(1);
    }

    printf("Servidor escuchando en el puerto %d...\n", PUERTO);

    while (1) {
        clilen = sizeof(cli_addr);

        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
            perror("Error en accept");
            continue;
        }

        printf("Conexión establecida con cliente: %s:%d\n",
               inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

        pid_t pid = fork();

        if (pid < 0) {
            perror("Error en fork");
            close(newsockfd);
        } else if (pid == 0) {
            close(sockfd);

            int fd_archivo = open(ARCHIVO, O_RDONLY);
            if (fd_archivo < 0) {
                perror("Error al abrir Google.html");
                close(newsockfd);
                exit(1);
            }

            char buffer[BUFFER_SIZE];
            ssize_t bytes_leidos, bytes_enviados;

            while ((bytes_leidos = read(fd_archivo, buffer, BUFFER_SIZE)) > 0) {
                bytes_enviados = write(newsockfd, buffer, bytes_leidos);
                if (bytes_enviados < 0) {
                    perror("Error al escribir en el socket");
                    break;
                }
            }

            if (bytes_leidos < 0) {
                perror("Error al leer Google.html");
            }

            close(fd_archivo);
            close(newsockfd);
            printf("Transferencia completada por el hijo (PID: %d).\n", getpid());
            exit(0);
        } else {

            close(newsockfd);
        }
    }

    close(sockfd);
    return 0;
}
