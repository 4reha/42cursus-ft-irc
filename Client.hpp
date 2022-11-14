#pragma once
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

#include <map>
#include <vector>

#define MAXCHAN	4

class Channel;
class Client
{
private:
public:

	int 								port;
	int 								sockfd;
	
	bool 								Registered;
	bool 								Authenticated;
	bool 								nicked;
	bool 								named;
	std::string 						ip_addr;
	std::string 						nickname;
	std::string 						username;
	struct pollfd 						sock_pollin;
	struct pollfd 						sock_pollout;
	std::vector<std::string> 			pending_msgs;
	std::map<std::string, Channel*> 	channels;

	Client(Client const &cpy);
	Client(struct sockaddr_in cli_addr, int newsockfd);
	void	leaveAllChans();
	~Client();
};

#include "Channel.hpp"
void	Client::leaveAllChans()
{
	for (std::map<std::string, Channel*>::iterator it = channels.begin(); it != channels.end(); it++)
		it->second->Members.erase(this);
	this->channels.clear();
}

Client::Client(struct sockaddr_in cli_addr, int newsockfd)
{
	this->ip_addr = inet_ntoa(cli_addr.sin_addr);
	this->port = ntohs(cli_addr.sin_port);
	this->sockfd = newsockfd;
	this->sock_pollin.fd = newsockfd;
	this->sock_pollout.fd = newsockfd;
	this->sock_pollin.events = POLLIN;
	this->sock_pollout.events = POLLOUT;
	this->Registered = false;
	this->Authenticated = false;
	this->nicked = false;
	this->named = false;
}

Client::Client(Client const &cpy)
{
	this->ip_addr = cpy.ip_addr;
	this->port = cpy.port;
	this->sockfd = cpy.sockfd;
	this->nickname = cpy.nickname;
	this->username = cpy.username;
	this->sock_pollin = cpy.sock_pollin;
	this->sock_pollout = cpy.sock_pollout;
	this->Registered = false;
	this->Authenticated = false;
	this->nicked = false;
	this->named = false;
}

Client::~Client()
{
	if (this->Registered)
		std::cout << "user " << this->nickname << " has disconnected!" << std::endl;
	close(this->sockfd);
}


