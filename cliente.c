//Juan David Morocho Angulo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PUERTO 9999
#define BUFFER_SIZE 1024

int conectar_servidor(const char *ip_servidor, int puerto){
    int sockfd;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET,SOCK_STREAM,0);

    if(sockfd < 0){
        perror("Error al crear el socket del cliente.");
        exit(1);
    }

    memset((char*) &serv_addr,0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(puerto);

    serv_addr.sin_addr.s_addr = inet_addr(ip_servidor);

    if(ser_addr.sin_addr.s_addr == INADDR_NONE){
        char msg_ip[] = "Dirección IP no válida\n";
        writen(STDERR_FILENO,msg_ip,strlen(msg_ip));
        close(sockfd);
        exit(1);
    }

    if(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        perror("Error al conectar con el servidor.");
        exit(1);
    }

    return sockfd;
}

void recibir_mostrar(int sockfd){
    char buffer[BUFFER_SIZE];
    ssize_t bytes_leidos;

    while((bytes_leidos = read(sockfd,buffer,BUFFER_SIZE)) > 0){
        if(write(STDOUT_FILENO,buffer,bytes_leidos) < 0){
            perror("Error al escribir por pantalla.");
            break;
        }
    }

    if(bytes_leidos < 0){
         perror("Error al leer el socket.");
    }
}

int main(int argc, char *argv[]) {
    int sockfd;
    
    if (argc < 2) {
        char msg_error[] = "Error: para usarse debe de realizar la siguiente acción ./cliente IP_Servidor\n";
        write(STDERR_FILENO, msg_error, strlen(msg_error));
        exit(1);
    }

    sockfd = conectar_servidor(argv[1],PUERTO);

    recibir_motrar(sockfd);
    
    close(sockfd);
    
    return 0;
}
