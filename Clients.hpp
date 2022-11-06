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
	struct pollfd sock_pollin;
	struct pollfd sock_pollout;

	bool	Registered;



	Clients();
	~Clients();
	void Setup_clients(sockaddr_in cli_addr, int newsockfd);
};

Clients::Clients()
{
	bzero(&this->sock_pollin, sizeof(this->sock_pollin));
	bzero(&this->sock_pollout, sizeof(this->sock_pollout));
	this->Registered = false;
}

Clients::~Clients()
{
	close(this->sockfd);
}

void Clients::Setup_clients(sockaddr_in cli_addr, int newsockfd)
{
	this->ip_addr = inet_ntoa(cli_addr.sin_addr);
	this->port = ntohs(cli_addr.sin_port);
	this->sockfd = newsockfd;
	this->sock_pollin.fd = this->sockfd;
	this->sock_pollout.fd = this->sockfd;
	this->sock_pollin.events = POLLIN;
	this->sock_pollout.events = POLLOUT;
	// std::cout << "New client connected from: " << this->ip_addr << ":" << this->port << std::endl;
}