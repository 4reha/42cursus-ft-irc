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
#include "Client.hpp"
// #include "Server.hpp"

class Client;
class Channel
{
private:
	/* data */
public:
	size_t						maxMembers;
	std::string 				Name;
	std::string 				Password;
	std::vector<Client *> 		Members;
	std::vector<Client *> 		Operators;
	std::vector<Client *> 		Banned;

	Channel(std::string name);
	~Channel();

	void broadcast_msg(std::string msg);
	bool isBanned(std::string user);
	bool isFull();
};

void	Channel::broadcast_msg(std::string msg)
{
	for (size_t i = 0; i < Members.size(); i++)
		Members[i]->pending_msgs.push_back(msg);
}

bool Channel::isBanned(std::string user)
{
	for (size_t i = 0; i < this->Banned.size(); i++)
		if (user == Banned[i]->nickname)
			return (true);
	return (false);
}

bool Channel::isFull()
{
	return (this->Members.size() == this->maxMembers);
}

Channel::Channel(std::string name)
{
	this->Name = name;
	this->maxMembers = 5;
}

Channel::~Channel()
{
}
