#pragma once
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <set>
#include <map>
#include <vector>
#include "Channel.hpp"


#define MAXCHAN	4

class Channel;
class Client
{
	private:
	public:

		int 								port;
		int 								sockfd;
		
		bool 								nicked;
		bool 								named;
		bool 								Registered;
		bool 								Authenticated;

		time_t 								logtime;

		std::string 						ip_addr;
		std::string 						nickname;
		std::string 						username;
		std::string                         userinfo;

		struct pollfd 						sock_pollin;
		struct pollfd 						sock_pollout;
		
		std::set<std::string> 				friends;
		std::vector<std::string> 			pending_msgs;
		std::map<std::string, Channel*> 	channels;

		~Client						();
		Client						(Client const &cpy);
		Client						(struct sockaddr_in cli_addr, int newsockfd);

		void    build				();
		void	leaveAllChans		(std::string msg);
		void	leaveChannel		(Channel* channel, std::string msg);
		bool    join_channel		(Channel* channel, std::string mode);
};


