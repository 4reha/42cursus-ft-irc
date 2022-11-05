#pragma	once
#include <iostream>
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

class Clients
{
private:
public:
	std::string ip_addr;
    int port;
	int sockfd;
	std::string nickname;
	std::string username;


	Clients(sockaddr_in cli_addr, int newsockfd);
	~Clients();
};

Clients::Clients(sockaddr_in cli_addr, int newsockfd)
{
	this->ip_addr = inet_ntoa(cli_addr.sin_addr);
	this->port = ntohs(cli_addr.sin_port);
	this->sockfd = newsockfd;
	// std::cout << "New client connected from: " << this->ip_addr << ":" << this->port << std::endl;
}