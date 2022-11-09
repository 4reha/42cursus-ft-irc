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
#include "Client.hpp"

bool ft_isspeacial(int c);
int read_sock(int newsockfd, std::string &cmd);

class Server
{
private:
	std::string password;
	struct sockaddr_in serv_addr;
	int port;
	int sockfd;
	std::vector<Client *> users_DB;
	std::vector<struct pollfd> poll_socket;

	void accept_connection();
	void name_later(int p_i);
	void cmd_manager(std::string cmd, int rp);

	void authenticate_connection(std::vector<std::string> &pass_cmd, int ui);
	void register_newClient(int ui);
	void deny_newClient(int ui, int n);

	void set_nickname(std::vector<std::string> &pass_cmd, int ui);
	void set_username(std::vector<std::string> &pass_cmd, int ui);

	void send_privmsg(std::vector<std::string> &pass_cmd, int ui);

	int search_client(std::string nickname);

public:
	Server(char **args);
	~Server();
	void init_Server();
	void start_connection();
	Client *new_client(struct sockaddr_in cli_addr, int newsockfd);
};

int Server::search_client(std::string nickname)
{
	for (size_t i = 0; i < this->users_DB.size(); i++)
		if (nickname == this->users_DB[i]->nickname)
			return (i);
	return (-1);
}

void Server::send_privmsg(std::vector<std::string> &pass_cmd, int ui)
{
	std::string format = ":" + this->users_DB[ui]->nickname + " PRIVMSG ";
	std::string msg;
	std::vector<std::string> receivers;

	for (char *token = std::strtok(const_cast<char *>(pass_cmd[1].c_str()), ","); token != NULL; token = std::strtok(nullptr, " "))
		receivers.push_back(token);
	for (size_t i = 2; i < pass_cmd.size(); i++)
		msg += " " + pass_cmd[i];
	for (size_t i = 0; i < receivers.size(); i++)
	{
		int p;
		if ((p = this->search_client(receivers[i])) >= 0)
		{
			this->users_DB[p]->pending_msgs.push_back(format + receivers[i] + msg + "\n");
		}
		else
			this->users_DB[ui]->pending_msgs.push_back("401 ERR_NOSUCHNICK " + receivers[i] + " :No such nick/channel\n");
	}
}

void Server::set_username(std::vector<std::string> &pass_cmd, int ui)
{
	if (pass_cmd.size() < 5)
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS <USER> :Not enough parameters\n");
	else
	{
		this->users_DB[ui]->username = pass_cmd[1];
		this->users_DB[ui]->named = true;
		if (this->users_DB[ui]->nicked && this->users_DB[ui]->named && !this->users_DB[ui]->Registered)
		{
			this->users_DB[ui]->Registered = true;
			std::cout << "New Client has connected from " << this->users_DB[ui]->ip_addr;
			std::cout << ":" << this->users_DB[ui]->port << " as: " << this->users_DB[ui]->nickname;
			std::cout << " id: " << ui << std::endl;
		}
	}
}

void Server::set_nickname(std::vector<std::string> &pass_cmd, int ui)
{
	std::string nickName;
	if (pass_cmd.size() < 2 || (pass_cmd.size() == 2 && pass_cmd[1].length() == 1 && pass_cmd[1][0] == ':'))
	{
		this->users_DB[ui]->pending_msgs.push_back("431 ERR_NONICKNAMEGIVEN :No nickname given\n");
		return;
	}
	if (pass_cmd.size() > 2)
	{
		nickName = std::string(pass_cmd[1], 1, pass_cmd[1].length() - 1);
		for (size_t i = 2; i < pass_cmd.size(); i++)
		{
			nickName += " ";
			nickName += pass_cmd[i];
		}
	}
	else
		nickName = pass_cmd[1];
	for (size_t i = 0; i < nickName.size(); i++)
	{
		if (!std::isalnum(nickName[i]) && !ft_isspeacial(nickName[i]))
		{
			this->users_DB[ui]->pending_msgs.push_back("432 ERR_ERRONEUSNICKNAME <" + nickName + "> :Erroneus nickname\n");
			return;
		}
	}
	for (size_t i = 0; i < this->users_DB.size(); i++)
	{
		if (nickName == this->users_DB[i]->nickname)
		{
			this->users_DB[ui]->pending_msgs.push_back("433 ERR_NICKNAMEINUSE <" + nickName + "> :Nickname is already in use\n");
			return;
		}
	}
	this->users_DB[ui]->nickname = nickName;
	this->users_DB[ui]->nicked = true;
	if (this->users_DB[ui]->nicked && this->users_DB[ui]->named && !this->users_DB[ui]->Registered)
	{
		this->users_DB[ui]->Registered = true;
		std::cout << "New Client has connected from " << this->users_DB[ui]->ip_addr;
		std::cout << ":" << this->users_DB[ui]->port << " as: " << this->users_DB[ui]->nickname;
		std::cout << " id: " << ui << std::endl;
	}
}

void Server::deny_newClient(int ui, int n)
{
	std::cout << "Connection refused from: " << users_DB[ui]->ip_addr;
	std::cout << ":" << users_DB[ui]->port << ", Using Password: ";
	if (n < 2)
	{
		std::cout << "NO!" << std::endl;
		users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS <PASS> :Not enough parameters\n");
	}
	else
	{
		std::cout << "YES!" << std::endl;
		users_DB[ui]->pending_msgs.push_back("464 ERR_PASSWDMISMATCH :Password incorrect\n");
	}
}

void Server::authenticate_connection(std::vector<std::string> &pass_cmd, int ui)
{
	std::string passcode;

	if (pass_cmd.size() > 2)
	{
		passcode = std::string(pass_cmd[1], 1, pass_cmd[1].length() - 1);
		for (size_t i = 2; i < pass_cmd.size(); i++)
		{
			passcode += " ";
			passcode += pass_cmd[i];
		}
	}
	else if (pass_cmd.size() == 2)
		passcode = pass_cmd[1];
	if (passcode == this->password)
		this->users_DB[ui]->Authenticated = true;
	else
		this->deny_newClient(ui, pass_cmd.size());
}

void Server::cmd_manager(std::string cmd, int rp)
{
	// std::cout << cmd << std::endl;
	std::string cpy = cmd;
	std::vector<std::string> cmd_out;
	for (char *token = std::strtok(const_cast<char *>(cmd.c_str()), " "); token != NULL; token = std::strtok(nullptr, " "))
		cmd_out.push_back(token);
	if (cmd_out[0] == "PASS")
	{
		if (this->users_DB[rp]->Registered)
			this->users_DB[rp]->pending_msgs.push_back("462 ERR_ALREADYREGISTRED :PASS may not reregister\n");
		else
			this->authenticate_connection(cmd_out, rp);
	}
	else if (this->users_DB[rp]->Authenticated && cmd_out[0] == "NICK")
	{
		this->set_nickname(cmd_out, rp);
	}
	else if (this->users_DB[rp]->Authenticated && cmd_out[0] == "USER")
	{
		if (this->users_DB[rp]->Registered)
			this->users_DB[rp]->pending_msgs.push_back("462 ERR_ALREADYREGISTRED :USER may not reregister\n");
		this->set_username(cmd_out, rp);
	}
	else if (this->users_DB[rp]->Registered)
	{
		// std::cout << cpy << std::endl;
		if (cmd_out[0] == "PRIVMSG")
		{
			this->send_privmsg(cmd_out, rp);
		}
	}
	else
	{
		this->users_DB[rp]->pending_msgs.push_back("451 ERR_NOTREGISTERED :You have not registered\n");
	}
}

void Server::name_later(int p_i)
{
	std::string cmd;
	int rp = (p_i - 1) / 2;
	if (!read_sock(users_DB[rp]->sockfd, cmd))
	{
		// close(users_DB[rp]->sockfd);
		delete users_DB[rp];
		this->users_DB.erase(users_DB.begin() + rp);
		this->poll_socket.erase(poll_socket.begin() + p_i);
		this->poll_socket.erase(poll_socket.begin() + p_i);
		for (size_t i = 0; i < this->users_DB.size(); i++)
			std::cout << "db: " << this->users_DB[i]->nickname << std::endl;
	}
	else
	{
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
	this->poll_socket.push_back(this->users_DB.back()->sock_pollin);  // unpair
	this->poll_socket.push_back(this->users_DB.back()->sock_pollout); // pair
}

Client *Server::new_client(struct sockaddr_in cli_addr, int newsockfd)
{
	return (new Client(cli_addr, newsockfd));
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
		for (int p = this->poll_socket.size() - 1; p >= 0; p--)
		{
			int t;
			if ((t = poll(&(this->poll_socket[p]), 1, 0)))
			{
				if (t == -1)
				{
					perror("Error: poll failed");
					exit(1);
				}
				if (!p)
				{
					this->accept_connection();
				}
				else if (p % 2)
				{ // if unpair
					this->name_later(p);
				}
				else if (!(p % 2))
				{ // if pair
					int rp = (p - 2) / 2;
					while (users_DB[rp]->pending_msgs.size())
					{
						send(users_DB[rp]->sockfd, users_DB[rp]->pending_msgs[0].c_str(), users_DB[rp]->pending_msgs[0].length(), 0);
						users_DB[rp]->pending_msgs.erase(users_DB[rp]->pending_msgs.begin());
					}
				}
			}
		}
	}
}
