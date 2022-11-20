#include "Bot.hpp"

Bot::Bot()
{
}

Bot::~Bot()
{
}

void	Bot::botcmd(std::vector<Client *>& clients, std::vector<std::string>& cmds, int ui)
{
	this->users = clients;
	if (cmds.size() >= 2)   {
		if (cmds[1] == ":logtime") {
			this->LOGTIMEcmd(cmds, ui);
		}
		else if (cmds[1] == ":location") {
			this->LOCATIONcmd(cmds, ui);
		}
		else if (cmds[1] == ":addfriend") {
			this->ADDFRIENDcmd(cmds, ui);
		}
		else if (cmds[1] == ":rmfriend") {
			this->RMFRIENDcmd(cmds, ui);
		}
		else if (cmds[1] == ":status") {
			this->STATUScmd(ui);
		}
		else if (cmds[1] == ":help") {
			this->HELPcmd(ui);
		}
	}
	else
        this->HELPcmd(ui);
}

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
	this->users[ui]->pending_msgs.push_back(format + std::to_string(days) + "d:" + std::to_string(hours)  + "h:" + std::to_string(min)  + "m:" + std::to_string(sec) + "s\n");
	std::cout << "logtime: " << days << ":" << hours << ":" << min << ":" << sec << std::endl;
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
	}
	std::string format = ":BOT NOTICE IRC :location for " + user->nickname + " is: ";
	std::cout << "ip: " << user->ip_addr << std::endl;
	if (user->ip_addr.substr(0, 4) == "127.")	{
		this->users[ui]->pending_msgs.push_back(":BOT NOTICE IRC :" + user->nickname + " is connected from the same host as the server!\n");
		return ;
	}
	if (user->ip_addr.substr(0, 4) != "10.1")	{
		this->users[ui]->pending_msgs.push_back(":BOT NOTICE IRC :" + user->nickname + " is not in the cluster\n");
		return ;
	}
	std::vector<std::string> ip;
	std::string ip_addr = user->ip_addr;
	for (char *token = std::strtok(const_cast<char *>(ip_addr.c_str()), "."); token != NULL; token = std::strtok(nullptr, "."))
		ip.push_back(token);
	location = "e" + std::string(1, ip[1][1]) + "r" + ip[2] + "p" + ip[3];
	this->users[ui]->pending_msgs.push_back(format + location + "\n");
}

void	Bot::ADDFRIENDcmd(std::vector<std::string>& cmds, int ui)
{
	if (cmds.size() < 3)	{
		this->users[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS ADDFRIENDcmdS :Not enough parameters\n");
		return ;
	}
	Client *user = search_client(cmds[2]);
	if (!user)	{
		this->users[ui]->pending_msgs.push_back("401 ERR_NOSUCHNICK " + cmds[2] + " :No such nick\n");
		return ;
	}
	if (this->users[ui]->friends.insert(cmds[2]).second)
		this->users[ui]->pending_msgs.push_back(":BOT NOTICE IRC :You have added "+ cmds[2] +" as friend\n");
}

void	Bot::RMFRIENDcmd(std::vector<std::string>& cmds, int ui)
{
	if (cmds.size() < 3)	{
		this->users[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS ADDFRIENDcmdS :Not enough parameters\n");
		return ;
	}
	if (this->users[ui]->friends.erase(cmds[2]))
		this->users[ui]->pending_msgs.push_back(":BOT NOTICE IRC :You have removed "+ cmds[2] +" from friends\n");
}

void	Bot::STATUScmd(int ui)
{
	size_t i;
	if (!this->users[ui]->friends.size())	{
		this->noFriends(ui);
		return ;
	}
	this->users[ui]->pending_msgs.push_back(":BOT NOTICE IRC :    NAME                    STATUS    \n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :------------------------------------\n");
	for (std::set<std::string>::iterator it = this->users[ui]->friends.begin(); it != this->users[ui]->friends.end() ;it++)	{
		for (i = 0; i < this->users.size(); i++)	{
			if (this->users[i]->nickname == *it)	{
				this->users[ui]->pending_msgs.push_back("NOTICE IRC :    " + set_strwidth(*it) + "Online\n");
				std::cout << *it + "                    Online" << std::endl;
				break ;
			}
		}
		if (i == this->users.size()) 
			this->users[ui]->pending_msgs.push_back("NOTICE IRC :    " + set_strwidth(*it) + "Offline\n");
	}
}

void	Bot::HELPcmd(int ui)
{

	this->users[ui]->pending_msgs.push_back(":BOT NOTICE IRC :Hello, I Am BOT.. And This What I Can Do:\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :BOT <cmd> {param}:\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :   *> :logtime {user}(optinal) -- Display Logtime\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :   *> :location {user}(optinal) -- Display Location\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :   *> :addfriend {user} -- Add Friend To Friends List\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :   *> :rmfriend {user} -- Remove Friend From Friends List\n");
	this->users[ui]->pending_msgs.push_back("NOTICE IRC :   *> :status -- Display Frinds Status Online/Offline\n");

}
