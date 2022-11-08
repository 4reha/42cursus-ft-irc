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
		std::string						password;
		struct sockaddr_in 				serv_addr;
		int 							port;
		int								sockfd;
		std::vector<Clients*> 			users_DB;
		std::vector<struct pollfd> 		poll_socket;

		void 		accept_connection();
		void		name_later(int p_i);
		void	 	cmd_manager(std::string cmd, int rp);

		void 		authenticate_connection(std::vector<std::string>& pass_cmd, int ui);
		void	 	register_newClient(int ui);
		void	 	deny_newClient(int ui, int n);

	public:
		Server(char **args);
		~Server();
		void 		init_Server();
		void 		start_connection();
		Clients*	new_client(struct sockaddr_in cli_addr, int newsockfd);
};

void Server::register_newClient(int ui)	
{
	std::cout << "New Client from: " << users_DB[ui]->ip_addr;
	std::cout << ":" << users_DB[ui]->port << ", Welcome!" << std::endl;
	this->users_DB[ui]->Registered = true;

}

void Server::deny_newClient(int ui, int n)	
{
	std::cout << "Connection refused from: " << users_DB[ui]->ip_addr;
	std::cout << ":" << users_DB[ui]->port << ", Using Password: ";
	if (n < 2)	{
		std::cout << "NO!" << std::endl;
		users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS <PASS> :Not enough parameters\n");
	}
	else	{
		std::cout << "YES!" << std::endl;
		users_DB[ui]->pending_msgs.push_back("464 ERR_PASSWDMISMATCH :Password incorrect\n");
	}
}

void Server::authenticate_connection(std::vector<std::string>& pass_cmd, int ui)	
{
	std::string passcode;

	if (pass_cmd.size() > 2)	{
		passcode = std::string(pass_cmd[1], 1, pass_cmd[1].length() - 1);
		for (size_t i = 2; i < pass_cmd.size(); i++)	{
			passcode += " ";
			passcode += pass_cmd[i];
		}
	}
	else if (pass_cmd.size() == 2)
		passcode = pass_cmd[1];
	std::cout << "TEST:" << passcode  << ":"<< std::endl;
	if (passcode == this->password)
		this->register_newClient(ui);
	else
		this->deny_newClient(ui, pass_cmd.size());
}

void Server::cmd_manager(std::string cmd, int rp)	
{

	std::cout << cmd << std::endl;
	std::vector<std::string> cmd_out;
	for (char *token = std::strtok(const_cast<char *>(cmd.c_str()), " "); token != NULL; token = std::strtok(nullptr, " "))
		cmd_out.push_back(token);
	if (cmd_out[0] == "PASS")	{
		if (this->users_DB[rp]->Registered)
			this->users_DB[rp]->pending_msgs.push_back("462 ERR_ALREADYREGISTRED :You may not reregister\n");
		else
			this->authenticate_connection(cmd_out, rp);
	}
	else if (this->users_DB[rp]->Registered)	{
	}
	else	{
		this->users_DB[rp]->pending_msgs.push_back("451 ERR_NOTREGISTERED :You have not registered\n");
	}
}

void Server::name_later(int p_i)	
{
	std::string cmd;
	int rp = (p_i - 1) / 2; 
	if (!read_sock(users_DB[rp]->sockfd, cmd))	{
		close(users_DB[rp]->sockfd);
		delete users_DB[rp];
		this->users_DB.erase(users_DB.begin() + rp);
		this->poll_socket.erase(poll_socket.begin() + p_i);
		this->poll_socket.erase(poll_socket.begin() + p_i);
	}
	else{
		// std::cout << "cmd : "<< cmd ;
		std::vector<std::string> args;
		for (char *token = std::strtok(const_cast<char *>(cmd.c_str()), "\r\n"); token != NULL; token = std::strtok(nullptr, "\r\n"))
			this->cmd_manager(token, rp);
	}
}

void Server::accept_connection()	
{
	int newsockfd;
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);

	if ((newsockfd = accept(this->sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1)
	{
		perror("Error: Accepting failure");
		exit(EXIT_FAILURE);
	}
	this->users_DB.push_back(this->new_client(cli_addr, newsockfd));
	this->poll_socket.push_back(this->users_DB.back()->sock_pollin); // unpair
	this->poll_socket.push_back(this->users_DB.back()->sock_pollout); // pair
}

Clients *Server::new_client(struct sockaddr_in cli_addr, int newsockfd)
{
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
	struct pollfd poll_sockfd;

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
	poll_sockfd.fd = this->sockfd;
	poll_sockfd.events = POLLIN;
	this->poll_socket.push_back(poll_sockfd);
}



void Server::start_connection() 
{
	while (1)
	{
		for (int p = this->poll_socket.size() - 1; p >= 0; p--)	{
			int t;
			if ((t = poll(&(this->poll_socket[p]), 1, 0)))	{
				if (t == -1)	{
					perror("Error: poll failed");
					exit(1);
				}
				if (!p)	{
					this->accept_connection();
				}
				else if (p % 2)	{ // if unpair
					this->name_later(p);
				}
				else if (!(p % 2 ))	{ // if pair
					int rp = (p - 2) / 2; 
					while (users_DB[rp]->pending_msgs.size()){
						send(users_DB[rp]->sockfd, users_DB[rp]->pending_msgs[0].c_str(), users_DB[rp]->pending_msgs[0].length(), 0);
						users_DB[rp]->pending_msgs.erase(users_DB[rp]->pending_msgs.begin());
					}
				}
			}
		}
	}
}
