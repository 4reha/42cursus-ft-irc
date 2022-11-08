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
	std::vector<std::string> pending_msgs;

	bool	Registered;
	bool	Authenticated;
	bool	nicked;
	bool	named;



	Clients();
	Clients(Clients const & cpy);
	Clients(struct sockaddr_in cli_addr, int newsockfd);
	~Clients();

};

Clients::Clients()
{
	bzero(&this->sock_pollin, sizeof(this->sock_pollin));
	bzero(&this->sock_pollout, sizeof(this->sock_pollout));
	this->Registered = false;
}

Clients::Clients(struct sockaddr_in cli_addr, int newsockfd)
{
	this->ip_addr = inet_ntoa(cli_addr.sin_addr);
	this->port = ntohs(cli_addr.sin_port);
	this->sockfd = newsockfd;
	this->sock_pollin.fd = newsockfd;
	this->sock_pollout.fd = newsockfd;
	this->sock_pollin.events = POLLIN;
	this->sock_pollout.events = POLLOUT;
	// std::cout << "New client connected from: " << this->ip_addr << ":" << this->port << std::endl;
}


Clients::Clients(Clients const & cpy)
{
	this->ip_addr = cpy.ip_addr;
    this->port = cpy.port;
    this->sockfd = cpy.sockfd;
    this->nickname = cpy.nickname;
    this->username = cpy.username;
    this->sock_pollin = cpy.sock_pollin;
    this->sock_pollout = cpy.sock_pollout;
    this->Registered = false;
}

Clients::~Clients()
{
	std::cout << "user " << this->nickname << " has disconnected!" << std::endl;
	close(this->sockfd);
}
