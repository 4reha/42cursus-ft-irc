#pragma once
#include <iostream>
#include <cmath>
#include <vector>
#include "Client.hpp"


std::string	set_strwidth(const std::string& str);
class Bot
{
	private:
		/* data */
	public:
		std::vector<Client *> 		users;

		Bot();
		~Bot();

		void		HELPcmd			(int ui);
		void		STATUScmd		(int ui);
		void		noFriends		(int ui);
		void		LOGTIMEcmd		(std::vector<std::string>& cmds, int ui);
		void		LOCATIONcmd		(std::vector<std::string>& cmds, int ui);
		void		ADDFRIENDcmd	(std::vector<std::string>& cmds, int ui);
		void		RMFRIENDcmd		(std::vector<std::string>& cmds, int ui);
		void		botcmd			(std::vector<Client *>& clients, std::vector<std::string>& cmds, int ui);
		
		Client*		search_client	(std::string nick);
};
