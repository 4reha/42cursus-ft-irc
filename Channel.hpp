#pragma once
#include <iostream>
#include <unistd.h>
#include <map>
#include <set>
#include "Client.hpp"

class Client;
bool 	isModeChar(int c);
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

		bool 	isFull			();
		bool	isInvitOnly		();
		bool	isMode			(char mode);
		bool 	isBanned		(Client* user);
		bool	isMember		(Client* user);
		bool	isOperator		(Client* user);
		bool	UserIsV			(Client* user);
		bool	isinvited		(Client* user);
		bool	setMemModes		(std::string m, std::string user);

		void 	remove_user		(Client* user);
		void	setModes		(std::string modes);
		void	setLimit		(char m, std::string limit);
		void	Bann			(std::string user, char op);
		void    setTopic		(std::vector<std::string> args);
		void 	broadcast_msg	(Client* sender, std::string msg);
		void	setupModes(Client* user, std::vector<std::string>& pass_cmd);
		
		std::string	getMode();
		std::string getUsers();
		std::string	getBanned();
};
