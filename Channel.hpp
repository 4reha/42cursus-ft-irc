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
#include "Server.hpp"

class Channel
{
	private:
		/* data */
	public:

		int									maxMembers;
		std::string 						Name;
		std::map<std::string, Client*> 		Members;
		std::map<std::string, Client*> 		Operators;
		std::map<std::string, Client*> 		Banned;
		
		Channel(std::string name);
		~Channel();
};

Channel::Channel(std::string name)
{
	this->Name = name;
	this->maxMembers = 5;
}

Channel::~Channel()
{
}
