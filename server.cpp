#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream> 
#include <poll.h>
#include <arpa/inet.h>

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
    std::cout << ntohs(serv_addr.sin_port) << std::endl;

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error_lol("LMAO2");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    int i = 0;
    struct pollfd mypoll;
    bzero(&mypoll, sizeof(mypoll));
    mypoll.fd = sockfd;
    mypoll.events = POLLIN;
    struct pollfd mypollfd[2];
    bzero(&mypollfd, sizeof(mypollfd));
    while (1){
        if (poll(&mypoll, 1, 0))
        {
            newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
            sockfds[i++] = newsockfd;
            if (newsockfd  < 0)
                error_lol("LMAO3");
            std::cout << "CLIENT: " << inet_ntoa(cli_addr.sin_addr) << "---" << ntohs(cli_addr.sin_port) << std::endl;
            mypollfd[0].fd = sockfds[0];
            mypollfd[1].fd = sockfds[1];
            mypollfd[0].events = POLLIN;
            mypollfd[1].events = POLLIN;
        }
        bzero(buffer, 1000);
        // sleep(2);
        if (poll(mypollfd, 2, 0))
        {
            if (read(sockfds[0], buffer, 1000) < 0)
                error_lol("LMAO4");
            if (sockfds[1] && read(sockfds[1], buffer, 1000) < 0)
                error_lol("LMAO4");
            std::cout << "client : " << buffer;
        }
        bzero(buffer, 1000);
        // write(newsockfd, "hello", 6);
        // if (!strncmp("Bye", buffer, 3))
        //     break;
    }
    close(newsockfd);
    close(sockfd);
    return 0;
}