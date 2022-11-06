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
#include "Clients.hpp"

std::string read_sock(int newsockfd);
class Server
{
private:
	std::string password;
	struct sockaddr_in serv_addr, cli_addr;
	int port, sockfd;
	std::vector<Clients> users_DB;

public:
	Server(char **args);
	~Server();
	void init_Server();
	void start_connection();
};

Server::Server(char **args)
{
	this->port = std::stoi(args[0]);
	this->password = args[1];
	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;
	this->serv_addr.sin_port = htons(port);
}

Server::~Server()
{
	std::cout << "Server Shuted down!" << std::endl;
	close(this->sockfd);
}

void Server::init_Server()
{
	if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Error: Socket failure");
		exit(EXIT_FAILURE);
	}
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Error: Binding failure");
		exit(EXIT_FAILURE);
	}
	if (listen(sockfd, 5) < 0)
	{
		perror("Error: Listen failure");
		exit(EXIT_FAILURE);
	}
}

void Server::start_connection()
{
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);
	int newsockfd;
	Clients user;
	std::string cmd;

	struct pollfd poll_sockfd;
	poll_sockfd.fd = this->sockfd;
	poll_sockfd.events = POLLIN;
	while (1)
	{
		if (poll(&poll_sockfd, 1, 0))
		{
			if ((newsockfd = accept(this->sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1)
			{
				perror("Error: Accepting failure");
				exit(EXIT_FAILURE);
			}
			user.Setup_clients(cli_addr, newsockfd);
			users_DB.push_back(user);
		}
		for (std::vector<Clients>::reverse_iterator rev_it = users_DB.rbegin(); rev_it != users_DB.rend(); rev_it++)
		{
			if (poll(&(rev_it->sock_pollin), 1, 0))
			{
				cmd = read_sock(newsockfd);
				std::cout << "cmd:||||||\n" << cmd << "||||||" << std::endl;
				std::vector<std::string> out;
				for (char *token = std::strtok(const_cast<char *>(cmd.c_str()), " "); token != NULL; token = std::strtok(nullptr, " "))
				{
					out.push_back(token);
				}
				if (out[0] == "PASS")	{
					if (out[1].substr(0, out[1].size() - 2) == this->password)
					{
						std::cout << "New Client from: " << user.ip_addr << ":" << user.port << ", Welcome!" << std::endl;
						rev_it->Registered = true;
					}
					else
					{
						std::cout << "Connection refused from: " << user.ip_addr << ":" << user.port << ", Password incorect!" << std::endl;
						users_DB.pop_back();
					}
				}
				// else if (out[0] == "NICK"){

				// }
			}
		}
	}
}
