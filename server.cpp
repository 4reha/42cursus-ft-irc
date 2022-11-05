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
    int sockfds[2] = {0};
    if (c != 2)
        error_lol("lmao");
    char buffer[1000];

    struct sockaddr_in serv_addr, cli_addr; 
    socklen_t clilen;
    int sockfd, portnbr, newsockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error_lol("LMAO1");

    portnbr = atoi(v[1]);

    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portnbr);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error_lol("LMAO2");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    int i = 0;
    while (1){
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        sockfds[i++] = newsockfd;
        if (newsockfd  < 0)
            error_lol("LMAO3");
        bzero(buffer, 1000);
        if (read(sockfds[0], buffer, 1000) <= 0)
            error_lol("LMAO4");
        std::cout << sockfds[0] << ", " << sockfds[1] << std::endl;
        if (sockfds[1] && read(sockfds[1], buffer, 1000) <= 0)
            error_lol("LMAO4");
        std::cout << "client : " << buffer;
        bzero(buffer, 1000);
        write(newsockfd, "hello", 6);
        if (!strncmp("Bye", buffer, 3))
            break;
    }
    close(newsockfd);
    close(sockfd);
    return 0;
}