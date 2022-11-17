#pragma once
#include "Server.hpp"

class Bot
{
	private:
		/* data */
	public:
		std::vector<Client *> 		users;

		Bot();
		~Bot();

		void    		botcmd(std::vector<Client *>& clients, std::vector<std::string>& cmds, int ui);
		Client*			search_client(std::string nick);
		void			LOGTIMEcmd(std::vector<std::string>& cmds, int ui);
		void			LOCATIONcmd(std::vector<std::string>& cmds, int ui);
		void			ADDFRIENDcmd(std::vector<std::string>& cmds, int ui);
		void			STATUScmd(int ui);
		void            noFriends(int ui);

		
};

void	Bot::noFriends(int ui)
{
	this->users[ui]->pending_msgs.push_back(":BOT NOTICE IRC :\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⡀⠴⠤⠤⠴⠄⡄⡀\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⠀⠀⣠⠄⠒⠉⠀⠀⠀⠀⠀⠀⠀⠀⠁⠃⠆⡄\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⢀⡜⠁⠀⠀⠀⢠⡄⠀⣀⠀⠀⠀⠀⠀⠀⠀⠀⠑⡄\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⢈⠁⠀⠀⠠⣿⠿⡟⣀⡹⠆⡿⣃⣰⣆⣤⣀⠀⠀⠹⡄\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⣼⠀⠀⢀⣀⣀⣀⣀⡈⠁⠙⠁⠘⠃⠡⠽⡵⢚⠱⠂⠛\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⡆⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢴⠀\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⡆⠀⠀⠀⠀⢐⣢⣤⣵⡄⢀⠀⢀⢈⣉⠉⠉⠒⠤⠀⠿\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠘⡇⠀⠀⠀⠀⠀⠉⠉⠁⠁⠈⠀⠸⢖⣿⣿⣷⠀⠀⢰⡆⠀⠀⠀⠀⠀GO MAKE SOME FRIENDS!!\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⡇⠀⠀⠀⠀⠀⠀⠀⠀⢀⠃⠀⡄⠀⠈⠉⠀⠀⠀⢴\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⢈⣇⠀⠀⠀⠀⠀⠀⠀⢰⠉⠀⠀⠱⠀⠀⠀⠀⠀⢠⡄⠀⠀⠀⠀⠀⣀⠔⠒⢒⡩⠃\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⣴⣿⣤⢀⠀⠀⠀⠀⠀⠈⠓⠒⠢⠔⠀⠀⠀⠀⠀⣶⠤⠄⠒⠒⠉⠁⠀⠀⠀⢸⡀\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⡄⠤⠒⠈⠈⣿⣿⣽⣦⠀⢀⢀⠰⢰⣀⣲⣿⡐⣤⠀⠀⢠⡾⠃⠀⠀⠀⠀⠀⠀⠀⣀⡄⣠⣵\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠘⠏⢿⣿⡁⢐⠶⠈⣰⣿⣿⣿⣿⣷⢈⣣⢰⡞⠀⠀⠀⠀⠀⠀⢀⡴⠋⠁\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⠀⠈⢿⣿⣍⠀⠀⠸⣿⣿⣿⣿⠃⢈⣿⡎⠁⠀⠀⠀⠀⣠⠞⠉\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⠀⠀⠈⢙⣿⣆⠀⠀⠈⠛⠛⢋⢰⡼⠁⠁⠀⠀⠀⢀⠔⠁\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠚⣷⣧⣷⣤⡶⠎⠛⠁⠀⠀⠀⢀⡤⠊\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠁⠈⠁⠀⠀⠀⠀⠀⠠⠊\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠃\n");
}

Client*	Bot::search_client(std::string nick)
{
	for (size_t i = 0; i < this->users.size(); i++)	{
		if (this->users[i]->nickname == nick)
			return this->users[i];
	}
	return (nullptr);
}

void	Bot::botcmd(std::vector<Client *>& clients, std::vector<std::string>& cmds, int ui)
{
	this->users = clients;
	if (cmds.size() >= 2)   {
		if (cmds[1] == "LOGTIME") {
			this->LOGTIMEcmd(cmds, ui);
		}
		else if (cmds[1] == "LOCATION") {
			this->LOCATIONcmd(cmds, ui);
		}
		else if (cmds[1] == "ADDFRIEND") {
			this->ADDFRIENDcmd(cmds, ui);
		}
		else if (cmds[1] == "STATUS") {
			this->STATUScmd(ui);
		}
	}
}

void	Bot::STATUScmd(int ui)
{
	size_t i;
	if (!this->users[ui]->friends.size())	{
		this->noFriends(ui);
		return ;
	}

	this->users[ui]->pending_msgs.push_back(":BOT NOTICE IRC :Your friends status are:\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :    NAME                    STATUS    \n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :--------------------------------------\n");
	for (std::set<std::string>::iterator it = this->users[ui]->friends.begin(); it != this->users[ui]->friends.end() ;it++)	{
		for (i = 0; i < this->users.size(); i++)	{
			if (this->users[i]->nickname == *it)	{
				this->users[ui]->pending_msgs.push_back("NOTICE IRC :    " + *it + "                    Online\n");
				std::cout << *it + "                    Online" << std::endl;
				break ;
			}
		}
		if (i == this->users.size()) 
			this->users[ui]->pending_msgs.push_back("NOTICE IRC :    " + *it + "                    Offline\n");
	}
}

void	Bot::ADDFRIENDcmd(std::vector<std::string>& cmds, int ui)
{
	std::cout << "ADDFRIENDcmd COMMANDS" << std::endl;
	if (cmds.size() < 3)	{
		this->users[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS ADDFRIENDcmdS :Not enough parameters\n");
		return ;
	}
	Client *user = search_client(cmds[2]);
	if (!user)	{
		this->users[ui]->pending_msgs.push_back("401 ERR_NOSUCHNICK " + cmds[2] + " :No such nick\n");
		return ;
	}
	this->users[ui]->friends.insert(cmds[2]);
	this->users[ui]->pending_msgs.push_back(":BOT NOTICE IRC :You have added "+ cmds[2] +" as friend\n");

	
}

void	Bot::LOCATIONcmd(std::vector<std::string>& cmds, int ui)
{
	Client *user;
	std::string location;
	if (cmds.size() >= 3)
		user = search_client(cmds[2]);
	else 
		user = this->users[ui];
	if (!user)	{
		this->users[ui]->pending_msgs.push_back("401 ERR_NOSUCHNICK " + cmds[2] + " :No such nick\n");
		return ;
	} //10.11.9.9 // 10.11.13.10
	std::string format = ":BOT NOTICE IRC :location for " + user->nickname + " is: ";
	if (user->ip_addr.substr(0, 4) == "127.")	{
		this->users[ui]->pending_msgs.push_back(":BOT NOTICE IRC :" + user->nickname + " is from connected from the same host as the server!\n");
		return ;
	}
	if (user->ip_addr.substr(0, 4) != "10.1")	{
		std::cout << "ip: " << user->ip_addr.substr(0, 4) << std::endl;
		this->users[ui]->pending_msgs.push_back(":BOT NOTICE IRC :" + user->nickname + " is not in the cluster\n");
		return ;
	}
	std::vector<std::string> ip;
	for (char *token = std::strtok(const_cast<char *>(user->ip_addr.c_str()), "."); token != NULL; token = std::strtok(nullptr, "."))
		ip.push_back(token);
	location = "e" + std::string(1, ip[1][1]) + "r" + ip[2] + "p" + ip[3];
	this->users[ui]->pending_msgs.push_back(format + location + "\n");

}

void	Bot::LOGTIMEcmd(std::vector<std::string>& cmds, int ui)
{
	Client *user;
	int		hours;
	int		days;
	int		min;
	int		sec;
	float	rest;
	if (cmds.size() >= 3)
		user = search_client(cmds[2]);
	else 
		user = this->users[ui];
	if (!user)	{
		this->users[ui]->pending_msgs.push_back("401 ERR_NOSUCHNICK " + cmds[2] + " :No such nick\n");
		return ;
	}
	time_t	now = time(NULL) - user->logtime;
	std::string format = ":BOT NOTICE IRC :logtime for " + user->nickname + " is: ";
	days = now / 86400;
	rest = fmod(now, 86400);
	hours = rest / 3600;
	rest = fmod(rest, 3600);
	min = rest / 60;
	rest = fmod(rest, 60);
	sec = rest;
	// this->users[ui]->pending_msgs.push_back("401 ERR_NOSUCHNICK " + this->users[ui]->nickname + " :No such nick\n");
	this->users[ui]->pending_msgs.push_back(format + std::to_string(days) + "d:" + std::to_string(hours)  + "h:" + std::to_string(min)  + "m:" + std::to_string(sec) + "s\n");
	std::cout << "logtime: " << days << ":" << hours << ":" << min << ":" << sec << std::endl;
}

Bot::Bot()
{
}

Bot::~Bot()
{
}
