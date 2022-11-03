#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream> 

void error_lol(std::string msg)
{
    std::cout << msg << std::endl;
    exit(1);
}

int main(int c, char **v)
{
    if (c != 3)
        error_lol("lol");
    
    char buffer[1000];
    int sockfd, portnbr;

    struct sockaddr_in serv_addr;
    struct hostent *server;
    
    portnbr = atoi(v[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error_lol("lol");
    server = gethostbyname(v[1]);
    if (!server)
        error_lol("lol");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portnbr);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)))
        error_lol("lol");
    
    while (1){
        bzero(buffer, 1000);
        fgets(buffer, 1000, stdin);
        write(sockfd, buffer, strlen(buffer));
        bzero(buffer, 1000);
        if (read(sockfd, buffer, 1000) < 0)
            error_lol("lol");
        std::cout << "server : " << buffer;
        if (!strncmp(buffer, "Bye", 3))
            break;
    }
    close(sockfd);
    return 0;
}