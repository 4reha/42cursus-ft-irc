#pragma once
#include <iostream>
#include <unistd.h>

#include <map>
#include <set>

class Client;
class Channel
{
private:
	/* data */
public:
	size_t								maxMembers;
	std::string 						Name;
	std::string 						Modes;
	std::string							Topic;
	std::string 						Password;
	std::map<Client *, std::string> 	Members;
	std::set<Client *> 					Invited;
	std::set<std::string> 				Banned;
	

	Channel(std::string name, std::string modes);
	~Channel();

	void 	broadcast_msg(Client* sender, std::string msg);
	bool 	isBanned(Client* user);
	bool 	isFull();
	void 	remove_user(Client* user);
	bool	isMember(Client* user);
	bool	isOperator(Client* user);
	void	setModes(std::string modes);
	bool	setMemModes(std::string m, std::string user);
	void	setLimit(char m, std::string limit);
	bool	isInvitOnly();
	void	Bann(std::string user, char op);
	void    setTopic(std::vector<std::string> args);
	bool	isMode(char mode);
	bool	UserIsV(Client* user);
	bool	isinvited(Client* user);
	

	std::string getUsers();
};

#include "Client.hpp"

bool	Channel::isinvited(Client* user)
{
	if (this->Invited.find(user) == this->Invited.end())
		return (false);
	return (true);
}

bool	Channel::UserIsV(Client* user)
{
	std::map<Client *,std::string>::iterator it = this->Members.find(user);
	if (it == this->Members.end())
        return (false);
	if (it->second.find('v') != it->second.npos)
		return (true);
	return (false);
}

bool	Channel::isMode(char mode)
{
	if (this->Modes.find(mode) != this->Modes.npos)
		return (true);
	return (false);
}

void	Channel::setTopic(std::vector<std::string> args)
{
	std::string topic;
	topic = args[2];
	for (size_t i = 3; i < args.size(); i++)
		topic += " " + args[i];
}

void	Channel::Bann(std::string user, char op)
{
	if (op == '+')	{
		this->Banned.insert(user);
		for (std::map<Client *,std::string>::iterator it = Members.begin(); it != Members.end(); it++)	{
			if (it->first->nickname == user)	{
				this->Members.erase(it);
				return ;
			}
		}
		std::cout << user << " is banned" << std::endl;
	}
	else
		this->Banned.erase(user);
}

void	Channel::setLimit(char m, std::string limit)
{
	if (m == '+')	{
		this->setModes("+l");
		this->maxMembers = std::stoi(limit);
	}
	else	{
		this->setModes("-l");
		this->maxMembers = -1;
	}
	
}

bool	Channel::setMemModes(std::string m, std::string user)
{
	for (std::map<Client *,std::string>::iterator it = Members.begin(); it != Members.end(); it++)	{
		if (it->first->nickname == user)	{
			if (m[0] == '+')	{
				if (it->second.find(m[1]) != it->second.npos)
					it->second += m[1];
			}
			else	{
				size_t p;
				if ((p = it->second.find(m[1])) == it->second.npos)
					it->second.erase(it->second.begin() + p);
			}
			return (true);
		}
	}
	return (false);
}

bool	Channel::isInvitOnly()
{
	return (this->Modes.find("i")!= std::string::npos);
}

void Channel::setModes(std::string modes)
{
	if (modes[0] == '+')	{
		for (size_t i = 1; i < modes.size(); i++)
			if ( this->Modes.find(modes[i]) == this->Modes.npos)
				this->Modes += modes[i];
	}
	else	{
		for (size_t i = 1, p; i < modes.size(); i++)	{
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
	for (std::set<std::string>::iterator it = this->Banned.begin(); it != this->Banned.end(); it++)
		if (user->nickname == *it)
			return (true);
	return (false);
}

bool Channel::isFull()
{
	return (this->maxMembers > 0 && this->Members.size() >= this->maxMembers);
}

Channel::Channel(std::string name, std::string modes)
{
	this->Name = name;
	this->maxMembers = -1;
	this->Modes = "+";
	this->setModes(modes);
}

Channel::~Channel()
{
}
