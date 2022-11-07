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

int read_sock(int newsockfd, std::string &cmd);

class Server
{
	private:
		std::string password;
		struct sockaddr_in serv_addr;
		int port, sockfd;
		std::vector<Clients*> users_DB;
		std::vector<pollfd> poll_socket;

	public:
		Server(char **args);
		~Server();
		void init_Server();
		void start_connection();
		Clients *new_client(sockaddr_in cli_addr, int newsockfd);
};

Clients *Server::new_client(sockaddr_in cli_addr, int newsockfd){
	return (new Clients(cli_addr, newsockfd));
}

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
	std::string cmd;

	struct pollfd poll_sockfd;
	poll_sockfd.fd = this->sockfd;
	poll_sockfd.events = POLLIN;
	this->poll_socket.push_back(poll_sockfd);
	while (1)
	{
		for (size_t p = 0; p < this->poll_socket.size(); p++)	{
			int t;
			if ((t = poll(&(this->poll_socket[p]), 1, 0)))	{
				if (t == -1)	{
					perror("Error: poll failed");
					exit(1);
				}
				if (!p)	{
					if ((newsockfd = accept(this->sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1)
					{
						perror("Error: Accepting failure");
						exit(EXIT_FAILURE);
					}
					this->users_DB.push_back(this->new_client(cli_addr, newsockfd));
					this->poll_socket.push_back(this->users_DB.front()->sock_pollin); // unpair
					this->poll_socket.push_back(this->users_DB.front()->sock_pollout); // pair
				}
				else if (p % 2)	{ // if unpair
					int rp = (p - 1) / 2; 
					if (!read_sock(users_DB[rp]->sockfd, cmd))	{
						close(users_DB[rp]->sockfd);
						delete users_DB[rp];
						users_DB.erase(users_DB.begin() + rp);
						this->poll_socket.erase(poll_socket.begin() + p);
						std::cout << "deleted! " << std::endl;
						this->poll_socket.erase(poll_socket.begin() + p);
					}
					else{
						std::cout << "cmd: "<< cmd ;
						cmd = "";
					}
				}
			}
		}
		// if (poll(&poll_sockfd, 1, 0))
		// {
		// 	if ((newsockfd = accept(this->sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1)
		// 	{
		// 		perror("Error: Accepting failure");
		// 		exit(EXIT_FAILURE);
		// 	}
		// 	this->users_DB.push_back(this->new_client(cli_addr, newsockfd));
		// }
		// for (int k = users_DB.size() - 1; k >= 0; k--)
		// {
		// 	if (poll(&(users_DB[k]->sock_pollin), 1, 0) > 0)
		// 	{
		// 		if (!read_sock(users_DB[k]->sockfd, cmd))	{
		// 			close(users_DB[k]->sockfd);
		// 			delete users_DB[k];
		// 			users_DB.erase(users_DB.begin() + k);
		// 		}
				// else	{
				// 	std::cout << "cmd: "<< cmd ;
		// 			// std::vector<std::string> out;
		// 			// for (char *token = std::strtok(const_cast<char *>(cmd.c_str()), " "); token != NULL; token = std::strtok(nullptr, " "))
		// 			// {
		// 			// 	out.push_back(token);
		// 			// }
		// 			// if (out[0] == "PASS")	{
		// 			// 	if (out[1].substr(0, out[1].size() - 2) == this->password)
		// 			// 	{
		// 			// 		std::cout << "New Client from: " << user.ip_addr << ":" << user.port << ", Welcome!" << std::endl;
		// 			// 		rev_it->Registered = true;
		// 			// 	}
		// 			// 	else
		// 			// 	{
		// 			// 		std::cout << "Connection refused from: " << user.ip_addr << ":" << user.port << ", Password incorect!" << std::endl;
		// 			// 		users_DB.pop_back();
		// 			// 	}
		// 			// }
		// 		}
		// 		cmd = "";
		// 	}
		// }
	}
}
