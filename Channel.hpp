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
#include <algorithm>

#include <map>
#include "Client.hpp"
// #include "Server.hpp"

class Client;
class Channel
{
private:
	/* data */
public:
	size_t								maxMembers;
	std::string 						Name;
	std::string 						Modes;
	std::string 						Password;
	std::map<Client *, std::string> 	Members;
	std::vector<Client *> 				Banned;

	Channel(std::string name, std::string modes);
	~Channel();

	void 	broadcast_msg(Client* sender, std::string msg);
	bool 	isBanned(Client* user);
	bool 	isFull();
	void 	remove_user(Client* user);
	bool	isMember(Client* user);
	bool	isOperator(Client* user);
	void	setModes(std::string modes, bool op);

	std::string getUsers();
};

void Channel::setModes(std::string modes, bool op) // 1 = + // 0 == -
{
	if (op)	{
		for (size_t i = 0; i < modes.size(); i++)
			if ( this->Modes.find(modes[i]) == this->Modes.npos)
				this->Modes += modes[i];
	}
	else	{
		for (size_t i = 0, p; i < modes.size(); i++)	{
			if ((p =this->Modes.find(modes[i])) != this->Modes.npos)
				this->Modes.erase(this->Modes.begin() + p);
		}
	}
}

std::string Channel::getUsers()
{
	std::string str = " :";
	for (std::map<Client *,std::string>::iterator it = Members.begin(); it != Members.end(); it++)	{
		if (this->isOperator(it->first))
			str += "@" + it->first->nickname + " ";
		else
			str += it->first->nickname + " ";
	}
	return (str);
}

bool	 Channel::isMember(Client* user)
{
	std::map<Client*,std::string>::iterator it = this->Members.find(user);
	if (it != this->Members.end())
        return (true);
	return (false);
}

bool	 Channel::isOperator(Client* user)
{
	std::map<Client *,std::string>::iterator it = this->Members.find(user);
	if (it == this->Members.end())
        return (false);
	if (it->second.find('o') != it->second.npos)
		return (true);
	return (false);
}

void Channel::remove_user(Client* user)
{
	this->Members.erase(user);
}

void	Channel::broadcast_msg(Client* sender, std::string msg)
{
	for (std::map<Client*,std::string>::iterator it = Members.begin(); it != Members.end(); it++)	{
		if (it->first != sender)
			it->first->pending_msgs.push_back(msg);
	}
}

bool Channel::isBanned(Client* user)
{
	for (size_t i = 0; i < this->Banned.size(); i++)
		if (user->nickname == Banned[i]->nickname)
			return (true);
	return (false);
}

bool Channel::isFull()
{
	return (this->maxMembers > 0 && this->Members.size() == this->maxMembers);
}

Channel::Channel(std::string name, std::string modes)
{
	this->Name = name;
	this->maxMembers = -1;
	this->setModes(modes, 1);
}

Channel::~Channel()
{
}
