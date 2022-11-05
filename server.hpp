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

#include <map>
#include "Clients.hpp"

class Server
{
private:
	std::string password;
	struct sockaddr_in serv_addr, cli_addr;
    int port, sockfd, newsockfd;
	std::map<std::string, Clients> users_DB;

public:
	Server(char** args);
	~Server();
	void	init_Server();
	void 	start_connection();
};

Server::Server(char** args)
{
	this->port = std::stoi(args[0]);
	this->password = args[1];
	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;
	this->serv_addr.sin_port = htons(port);
}

Server::~Server()
{
}

void	Server::init_Server()
{
	if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	{
		perror("Error: Socket failure");
		exit(EXIT_FAILURE);
	}
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)	{
		perror("Error: Binding failure");
		exit(EXIT_FAILURE);
	}
	if (listen(sockfd, 5) < 0)	{
		perror("Error: Listen failure");
		exit(EXIT_FAILURE);
	}
}

void	Server::start_connection()
{
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	int	newsockfd;

	struct pollfd socket_fd;
    socket_fd.fd = this->sockfd;
    socket_fd.events = POLLIN;
	while (1)	{
		if (poll(&socket_fd,1, 0))
		{
			newsockfd = accept(this->sockfd, (struct sockaddr *)&cli_addr, &clilen);
			// Clients user(cli_addr, newsockfd);
		}

	}

}


