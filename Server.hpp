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

#include "Channel.hpp"

bool	is_channel_prefix(int c);
bool 	isModeChar(int c);
void 	error_lol(std::string msg);
bool 	ft_isspeacial(int c);
int 	read_sock(int newsockfd, std::string &cmd);


// class Client;
// class Channel;
class Server
{
private:
	int 								port;
	int 								sockfd;
	std::string 						password;
	struct pollfd 						poll_sockfd;
	struct sockaddr_in 					serv_addr;
	std::vector<Client *> 				users_DB;
	std::map<std::string, Channel *> 	channel_DB;
	std::vector<struct pollfd> 			poll_socket;

	void	accept_connection();
	void	name_later(int p_i);
	void	cmd_manager(std::string cmd, int rp);	
	void	PASScmd(std::vector<std::string> &pass_cmd, int ui);
	void	register_newClient(int ui);
	void	deny_newClient(int ui, int n);

	void	NICKcmd(std::vector<std::string> &pass_cmd, int ui);
	void	USERcmd(std::vector<std::string> &pass_cmd, int ui);	
	void	PRIVMSGcmd(std::vector<std::string> &pass_cmd, int ui);
	void	NOTICEcmd(std::vector<std::string> &pass_cmd, int ui);
	void	JOINcmd(std::vector<std::string> &pass_cmd, int ui);
	void	MODEcmd(std::vector<std::string> &pass_cmd, int ui);
	void	TOPICcmd(std::vector<std::string> &pass_cmd, int ui);
	void	KICKcmd(std::vector<std::string> &pass_cmd, int ui);
ma	void	INVITEcmd(std::vector<std::string> &pass_cmd, int ui);
	void	PARTcmd(std::vector<std::string> &pass_cmd, int ui);

	int		search_client(std::string nickname);
	void	disconnect_client(int ui);
	void	welcome_client(int ui);

public:
	Server();
	~Server();
	void	setup_server(char ** args);
	void 	init_Server();
	void 	start_connection();
	Client *new_client(struct sockaddr_in cli_addr, int newsockfd);
};

void	Server::PARTcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::string msg = " " + this->users_DB[ui]->nickname;
	std::string format = ":" + this->users_DB[ui]->userinfo + " PART ";
	std::vector<std::string> channels;
	if (pass_cmd.size() < 2)	{
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS PART :Not enough parameters\n");
		return ;
	}
	else if (pass_cmd.size() > 2)	{
		msg = pass_cmd[2];
		for (size_t i = 3; i < pass_cmd.size(); i++)
			msg += " " + pass_cmd[i];
	}
	for (char *token = std::strtok(const_cast<char *>(pass_cmd[1].c_str()), ","); token != NULL; token = std::strtok(nullptr, ","))
		channels.push_back(token);
	std::map<std::string, Channel*>::iterator it;
	for (size_t i = 0; i < channels.size(); i++)	{
		if ((it = channel_DB.find(channels[i])) == channel_DB.end())
			this->users_DB[ui]->pending_msgs.push_back("403 ERR_NOSUCHCHANNEL " + channels[i] + " :No such channel\n");
		else if (!it->second->isMember(this->users_DB[ui]))
			this->users_DB[ui]->pending_msgs.push_back("442 ERR_NOTONCHANNEL " + channels[i] + " :You're not on that channel\n");
		else	{
			it->second->broadcast_msg(nullptr, format + channels[i] + msg + "\n");
			it->second->remove_user(this->users_DB[ui]);
			this->users_DB[ui]->channels.erase(channels[i]);
		}
	}
}

void	Server::INVITEcmd(std::vector<std::string> &pass_cmd, int ui)
{
	int	p;
	std::map<std::string, Channel*>::iterator it;

	if (pass_cmd.size() < 3)
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS INVITE :Not enough parameters\n");
	else if ((p = this->search_client(pass_cmd[1])) < 0)
		this->users_DB[ui]->pending_msgs.push_back("401 ERR_NOSUCHNICK <" + pass_cmd[1] + "> :No such nick/channel\n");
	else if ((it = this->channel_DB.find(pass_cmd[2])) != this->channel_DB.end() && !it->second->isMember(this->users_DB[ui]))
		this->users_DB[ui]->pending_msgs.push_back("442 ERR_NOTONCHANNEL " + pass_cmd[1] + " :You're not on that channel\n");
	else if (it != this->channel_DB.end() && it->second->isMember(this->users_DB[p]))
		this->users_DB[ui]->pending_msgs.push_back("443 ERR_USERONCHANNEL " + pass_cmd[1] + " " + pass_cmd[2] +" :is already on channel\n");
	else if (it != this->channel_DB.end() && it->second->isMode('i') && !it->second->isOperator(this->users_DB[ui]))
		this->users_DB[ui]->pending_msgs.push_back("482 ERR_CHANOPRIVSNEEDED " + pass_cmd[1] + " :You're not channel operator\n");
	else if (it == this->channel_DB.end())
		return ;
	else {
		it->second->Invited.insert(this->users_DB[p]);
		this->users_DB[ui]->pending_msgs.push_back("341 RPL_INVITING " + pass_cmd[1] + " " + pass_cmd[2] + "\n");
		this->users_DB[p]->pending_msgs.push_back(":" + this->users_DB[ui]->userinfo + " INVITE " + pass_cmd[1] + " " + pass_cmd[2] + "\n");
	}
}

void	Server::KICKcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::string msg = " " + this->users_DB[ui]->nickname;
	if (pass_cmd.size() < 3)	{
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS TOPIC :Not enough parameters\n");
		return ;
	}
	if (pass_cmd.size() >= 4)	{
		msg = " " + pass_cmd[3];
		for (size_t i = 4; i < pass_cmd.size(); i++)
			msg += " " + pass_cmd[i];
		msg += "\n";
	}
	std::map<std::string, Channel*>::iterator it;
	std::string format = ":" + this->users_DB[ui]->userinfo + " KICK ";
	std::vector<std::string>	channels;
	std::vector<std::string>	users;
	for (char *token = std::strtok(const_cast<char *>(pass_cmd[1].c_str()), ","); token != NULL; token = std::strtok(nullptr, ","))
		channels.push_back(token);
	for (char *token = std::strtok(const_cast<char *>(pass_cmd[2].c_str()), ","); token != NULL; token = std::strtok(nullptr, ","))
		users.push_back(token);
	for (size_t i = 0,  j = 0; i < channels.size() && j < users.size(); i++, j++)	{
		int l = this->search_client(users[j]);
		if ((it = channel_DB.find(channels[i])) == channel_DB.end())
			this->users_DB[ui]->pending_msgs.push_back("403 ERR_NOSUCHCHANNEL " + channels[i] + " :No such channel\n");
		else if (!it->second->isMember(this->users_DB[ui]))
			this->users_DB[ui]->pending_msgs.push_back("442 ERR_NOTONCHANNEL " + channels[i] + " :You're not on that channel\n");
		else if (!it->second->isOperator(this->users_DB[ui]))
			this->users_DB[ui]->pending_msgs.push_back("482 ERR_CHANOPRIVSNEEDED " + channels[i] + " :You're not channel operator\n");
		else if (l >= 0)	{
			if (it->second->isMember(this->users_DB[l]))	{
				it->second->broadcast_msg(nullptr, format + " " + channels[i] + " " + users[j] + msg);
				it->second->remove_user(this->users_DB[l]);
				this->users_DB[l]->channels.erase(channels[i]);
			}
		}
		else
			this->users_DB[ui]->pending_msgs.push_back("441 ERR_USERNOTINCHANNEL " + users[j] + " :They aren't on " + channels[i] + "\n");
		if (channels.size() == 1)
			i--;
	}
}

void	Server::welcome_client(int ui)
{
	this->users_DB[ui]->Registered = true;
	this->users_DB[ui]->build();
	std::cout << "New Client has connected from " << this->users_DB[ui]->ip_addr;
	std::cout << ":" << this->users_DB[ui]->port << " as: " << this->users_DB[ui]->nickname;
	std::cout << " id: " << ui << std::endl;
	this->users_DB[ui]->pending_msgs.push_back("001 " + this->users_DB[ui]->nickname + " :Welcome to the Internet Relay Network " + this->users_DB[ui]->userinfo + "\n");
	this->users_DB[ui]->pending_msgs.push_back("002 " + this->users_DB[ui]->nickname + " :Your host is e-LEMON-tor, running version 1.12\n");
	this->users_DB[ui]->pending_msgs.push_back("003 " + this->users_DB[ui]->nickname + " :This server was created 15-11-2002\n");
	this->users_DB[ui]->pending_msgs.push_back("004 " + this->users_DB[ui]->nickname + " :e-LEMON-ator 1.12 0 *\n");

}

void	Server::TOPICcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::map<std::string, Channel*>::iterator it;
	if (pass_cmd.size() < 2)
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS TOPIC :Not enough parameters\n");
	else if ((it = channel_DB.find(pass_cmd[1])) == channel_DB.end())
		this->users_DB[ui]->pending_msgs.push_back("403 ERR_NOSUCHCHANNEL " + pass_cmd[1] + " :No such channel\n");
	else if (!it->second->isMember(this->users_DB[ui]))
		this->users_DB[ui]->pending_msgs.push_back("442 ERR_NOTONCHANNEL " + pass_cmd[1] + " :You're not on that channel\n");
	else if (!it->second->isOperator(this->users_DB[ui]))
		this->users_DB[ui]->pending_msgs.push_back("482 ERR_CHANOPRIVSNEEDED " + pass_cmd[1] + " :You're not channel operator\n");
	else if (!it->second->isMode('t'))
		this->users_DB[ui]->pending_msgs.push_back("477 ERR_NOCHANMODES " + pass_cmd[1] + ":Channel doesn't support (t) mode\n");
	else if (pass_cmd.size() >= 2)
		it->second->setTopic(pass_cmd);
	else if (it->second->Topic == "")
		this->users_DB[ui]->pending_msgs.push_back("331 RPL_NOTOPIC " + pass_cmd[1] + " :No topic is set\n");
	else
		this->users_DB[ui]->pending_msgs.push_back("332 RPL_TOPIC " + pass_cmd[1] + it->second->Topic+"\n");
}

void	Server::MODEcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::string format = ":" + this->users_DB[ui]->userinfo + " MODE " + pass_cmd[1] + " ";
	size_t p = 3;
	if (pass_cmd.size() < 2)	{
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS MODE :Not enough parameters\n");
		return ;
	}
	std::map<std::string, Channel*>::iterator it;
	if ((it = channel_DB.find(pass_cmd[1])) == channel_DB.end())	{
		this->users_DB[ui]->pending_msgs.push_back("403 ERR_NOSUCHCHANNEL " + pass_cmd[1] + " :Not enough parameters\n");
		return ;
	}
	if (!it->second->isMember(this->users_DB[ui]))	{
		this->users_DB[ui]->pending_msgs.push_back("442 ERR_NOTONCHANNEL " + pass_cmd[1] + " :You're not on that channel\n");
		return ;
	}
	if (pass_cmd.size() == 2)	{
		std::string params = it->second->Modes;
		for (size_t i = 0; i < it->second->Modes.length(); i++)
		{
			if (it->second->Modes[i] == 'l')
				params += " " + std::to_string(it->second->maxMembers);
			if (it->second->Modes[i] == 'k')
				params += " " + it->second->Password;
		}	
		this->users_DB[ui]->pending_msgs.push_back("324 RPL_CHANNELMODEIS " + pass_cmd[1] + " " + params + "\n");
		return ;
	}
	if (!it->second->isOperator(this->users_DB[ui]))	{
		this->users_DB[ui]->pending_msgs.push_back("482 ERR_CHANOPRIVSNEEDED " + pass_cmd[1] + " :You're not channel operator\n");
		return ;
	}
	if (pass_cmd[2][0] == '+' || pass_cmd[2][0] == '-')	{
		for (size_t i = 1; i < pass_cmd[2].length(); i++)	{
			if (!isModeChar(pass_cmd[2][i]))	{
				this->users_DB[ui]->pending_msgs.push_back("472 ERR_UNKNOWNMODE " + std::string(1, pass_cmd[2][i]) + " :is unknown mode char to me for " + pass_cmd[1] + "\n");
				pass_cmd[2].erase(pass_cmd[2].begin() + i);
				continue ;
			}
			else if (pass_cmd[2][i] == 'o' || pass_cmd[2][i] == 'v')	{
				if (p < pass_cmd.size())	{
					if (!it->second->setMemModes(pass_cmd[2][0] + std::string(1, pass_cmd[2][i]), pass_cmd[p++]))
						this->users_DB[ui]->pending_msgs.push_back("441 ERR_USERNOTINCHANNEL "+ pass_cmd[p -1] + " :They aren't on " + pass_cmd[0] + "\n");
				}
				else
					this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS MODE :Not enough parameters\n");
			}
			else if (pass_cmd[2][i] == 'l')	{
				if (p < pass_cmd.size() || pass_cmd[2][0] == '-')
					it->second->setLimit(pass_cmd[2][0], pass_cmd[p++]);
				else if (pass_cmd[2][0] == '+')
					this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS MODE :Not enough parameters\n");
			}
			else if (pass_cmd[2][i] == 'k')	{
				if (p < pass_cmd.size())	{
					if (pass_cmd[2][0] == '-')	{
						if (it->second->Password == pass_cmd[p++])
							it->second->Password = "";
						else
							this->users_DB[ui]->pending_msgs.push_back("475 ERR_BADCHANNELKEY " + pass_cmd[1] + " :Cannot change key (-k)\n");
					}
					else	{
						if (it->second->Password == "")
							it->second->Password = pass_cmd[p++];
						else
							this->users_DB[ui]->pending_msgs.push_back("467 ERR_KEYSET " + pass_cmd[1] + " :Channel key already set\n");
					}	
				}
				else
					this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS MODE :Not enough parameters\n");
			}
			else if (pass_cmd[2][i] == 'b')	{
				if (p < pass_cmd.size())
					it->second->Bann(pass_cmd[p++], pass_cmd[2][0]);
				else
					this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS MODE :Not enough parameters\n");
			}
			else	{
				it->second->setModes(pass_cmd[2][0] + std::string(1, pass_cmd[2][i]));
			}
			it->second->broadcast_msg(nullptr, format + pass_cmd[2][0] + pass_cmd[2][i] + "\n");
			std::cout << format + pass_cmd[2][0] + pass_cmd[2][i] << std::endl;
		}
	}
}

void	Server::disconnect_client(int ui)
{
	for (std::map<std::string, Channel*>::iterator it = this->users_DB[ui]->channels.begin(); it != this->users_DB[ui]->channels.end(); it++)
		it->second->remove_user(this->users_DB[ui]);
	delete users_DB[ui];
	this->users_DB.erase(users_DB.begin() + ui);
	this->poll_socket.erase(poll_socket.begin() + (ui * 2 +1));
	this->poll_socket.erase(poll_socket.begin() + (ui * 2 +1));

}

void	Server::JOINcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::vector<std::string> channels;
	std::vector<std::string> keys;
	std::string join_msg = ":" + this->users_DB[ui]->userinfo+ " JOIN ";
	if (pass_cmd.size() < 2)
	{
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS <JOIN> :Not enough parameters\n");
		return;
	}
	if (pass_cmd[1] == "0")	{
		this->users_DB[ui]->leaveAllChans();
		return ;
	}
	for (char *token = std::strtok(const_cast<char *>(pass_cmd[1].c_str()), ","); token != NULL; token = std::strtok(nullptr, ","))
		channels.push_back(token);
	if (pass_cmd.size() > 2)
		for (char *token = std::strtok(const_cast<char *>(pass_cmd[2].c_str()), ","); token != NULL; token = std::strtok(nullptr, ","))
			keys.push_back(token);
	for (size_t i = 0; i < channels.size(); ++i)
	{
		if (channels[i][0] != '#' && channels[i][0] != '&' && channels[i][0] != '!' && channels[i][0] != '+')	{
			this->users_DB[ui]->pending_msgs.push_back("403 ERR_NOSUCHCHANNEL " + channels[i] + " :Unknown channel category\n");
			continue;
		}
		Channel *tmp_chn = new Channel(channels[i], "+");
		std::pair<std::map<std::string, Channel *>::iterator, bool> pair = this->channel_DB.insert(std::make_pair(channels[i], tmp_chn));
		if (pair.second)
		{
			if (this->users_DB[ui]->channels.size() == MAXCHAN)
				this->users_DB[ui]->pending_msgs.push_back("405 ERR_TOOMANYCHANNELS " + channels[i] + " :You have joined too many channels\n");
			else	{
				pair.first->second->Members.insert(std::make_pair(this->users_DB[ui], std::string("Oo")));
				if (i < keys.size())	{
					pair.first->second->setModes("+k");
					pair.first->second->Password = keys[i];
				}
				this->users_DB[ui]->channels.insert(*pair.first);
				pair.first->second->broadcast_msg(nullptr, join_msg + channels[i] + "\n");
				this->users_DB[ui]->pending_msgs.push_back("353 " + this->users_DB[ui]->nickname + " = " + channels[i] + pair.first->second->getUsers() + "\n");
				this->users_DB[ui]->pending_msgs.push_back("366 " + this->users_DB[ui]->nickname + " " + channels[i] + " :End of /NAMES list\n");
			}
		}
		else
		{
			if (pair.first->second->isInvitOnly() && !pair.first->second->isinvited(this->users_DB[ui]))
				this->users_DB[ui]->pending_msgs.push_back("473 ERR_INVITEONLYCHAN " + channels[i] + " :Cannot join channel (+i)\n");
			else if (pair.first->second->isBanned(this->users_DB[ui]))
				this->users_DB[ui]->pending_msgs.push_back("474 ERR_BANNEDFROMCHAN " + channels[i] + " :Cannot join channel (+b)\n");
			else if (pair.first->second->isFull())
				this->users_DB[ui]->pending_msgs.push_back("471 ERR_CHANNELISFULL " + channels[i] + " :Cannot join channel (+l)\n");
			else if (this->users_DB[ui]->channels.size() == MAXCHAN)
				this->users_DB[ui]->pending_msgs.push_back("405 ERR_TOOMANYCHANNELS " + channels[i] + " :You have joined too many channels\n");
			else if ((i < keys.size() && pair.first->second->Password == keys[i]) || (i >= keys.size() && pair.first->second->Password == ""))	{
				if (pair.first->second->Members.insert(std::make_pair(this->users_DB[ui], std::string(""))).second)	{
					this->users_DB[ui]->channels.insert(*pair.first);
					pair.first->second->broadcast_msg(nullptr, join_msg + channels[i] + "\n");
					this->users_DB[ui]->pending_msgs.push_back("353 " + this->users_DB[ui]->nickname + " = " + channels[i] + pair.first->second->getUsers() + "\n");
					this->users_DB[ui]->pending_msgs.push_back("366 " + this->users_DB[ui]->nickname + " " + channels[i] + " :End of /NAMES list\n");
				}
			}
			else
				this->users_DB[ui]->pending_msgs.push_back("475 ERR_BADCHANNELKEY " + channels[i] + " :Cannot join channel (+k)\n");
			delete tmp_chn;
		}
	}
}

int		Server::search_client(std::string nickname)
{
	for (size_t i = 0; i < this->users_DB.size(); i++)
		if (nickname == this->users_DB[i]->nickname)
			return (i);
	return (-1);
}

void	Server::NOTICEcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::string format = ":" + this->users_DB[ui]->nickname + " NOTICE ";
	std::string msg;
	std::vector<std::string> receivers;

	for (char *token = std::strtok(const_cast<char *>(pass_cmd[1].c_str()), ","); token != NULL; token = std::strtok(nullptr, ","))
		receivers.push_back(token);
	for (size_t i = 2; i < pass_cmd.size(); i++)
		msg += " " + pass_cmd[i];
	for (size_t i = 0; i < receivers.size(); i++)
	{
		int p;
		std::map<std::string, Channel*>::iterator it;
		if (receivers[i][0] != '#' && (p = this->search_client(receivers[i])) >= 0)
			this->users_DB[p]->pending_msgs.push_back(format + receivers[i] + msg + "\n");
		else if (receivers[i][0] == '#' && (it = this->users_DB[ui]->channels.find(receivers[i])) != this->users_DB[ui]->channels.end())
			it->second->broadcast_msg(this->users_DB[ui], format + receivers[i] + msg + "\n");
	}
}

void	Server::PRIVMSGcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::string format = ":" + this->users_DB[ui]->userinfo + " PRIVMSG ";
	std::string msg;
	std::vector<std::string> receivers;

	if (pass_cmd.size() < 3)	{
		this->users_DB[ui]->pending_msgs.push_back("412 ERR_NOTEXTTOSEND :No text to send\n");
		return ;
	}
	for (char *token = std::strtok(const_cast<char *>(pass_cmd[1].c_str()), ","); token != NULL; token = std::strtok(nullptr, ","))
		receivers.push_back(token);
	if (pass_cmd.size()== 3)
		msg = " :";
	for (size_t i = 2; i < pass_cmd.size(); i++)
		msg += " " + pass_cmd[i];
	for (size_t i = 0; i < receivers.size(); i++)
	{
		int p;
		std::map<std::string, Channel*>::iterator it;
		if (!is_channel_prefix(receivers[i][0]) && (p = this->search_client(receivers[i])) >= 0)
			this->users_DB[p]->pending_msgs.push_back(format + receivers[i] + msg + "\n");
		else if (is_channel_prefix(receivers[i][0]) && (it = this->channel_DB.find(receivers[i])) != this->channel_DB.end())
		{
			if (it->second->isMode('m'))
			{
				if (it->second->UserIsV(this->users_DB[ui]))
					it->second->broadcast_msg(this->users_DB[ui], format + receivers[i] + msg + "\n");
			}
			else {
				if (it->second->isMode('n'))	{
					if (it->second->isMember(this->users_DB[ui]))
						it->second->broadcast_msg(this->users_DB[ui], format + receivers[i] + msg + "\n");
					else
						this->users_DB[ui]->pending_msgs.push_back("404 ERR_CANNOTSENDTOCHAN <" + receivers[i] + "> :Cannot send to channel\n");
				}
				else
					it->second->broadcast_msg(this->users_DB[ui], format + receivers[i] + msg + "\n");
			}
		}
		else
			this->users_DB[ui]->pending_msgs.push_back("401 ERR_NOSUCHNICK <" + receivers[i] + "> :No such nick/channel\n");
	}
}

void	Server::USERcmd(std::vector<std::string> &pass_cmd, int ui)
{
	if (pass_cmd.size() < 5)
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS <USER> :Not enough parameters\n");
	else
	{
		this->users_DB[ui]->username = pass_cmd[1];
		this->users_DB[ui]->named = true;
		if (this->users_DB[ui]->nicked && this->users_DB[ui]->named && !this->users_DB[ui]->Registered)
			welcome_client(ui);
	}
}

void	Server::NICKcmd(std::vector<std::string> &pass_cmd, int ui)
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
		welcome_client(ui);
}

void	Server::deny_newClient(int ui, int n)
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

void	Server::PASScmd(std::vector<std::string> &pass_cmd, int ui)
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

void	Server::cmd_manager(std::string cmd, int rp)
{
	std::cout << cmd << std::endl;
	std::string cpy = cmd;
	std::vector<std::string> cmd_out;
	for (char *token = std::strtok(const_cast<char *>(cmd.c_str()), " "); token != NULL; token = std::strtok(nullptr, " "))
		cmd_out.push_back(token);
	if (cmd_out[0] == "PASS")
	{
		if (this->users_DB[rp]->Registered)
			this->users_DB[rp]->pending_msgs.push_back("462 ERR_ALREADYREGISTRED :PASS may not reregister\n");
		else
			this->PASScmd(cmd_out, rp);
	}
	else if (this->users_DB[rp]->Authenticated && cmd_out[0] == "NICK")
	{
		this->NICKcmd(cmd_out, rp);
	}
	else if (this->users_DB[rp]->Authenticated && cmd_out[0] == "USER")
	{
		if (this->users_DB[rp]->Registered)
			this->users_DB[rp]->pending_msgs.push_back("462 ERR_ALREADYREGISTRED :USER may not reregister\n");
		this->USERcmd(cmd_out, rp);
	}
	else if (cmd_out[0] == "QUIT")
		this->disconnect_client(rp);
	else if (this->users_DB[rp]->Registered)
	{
		// std::cout << cpy << std::endl;
		if (cmd_out[0] == "PRIVMSG")
			this->PRIVMSGcmd(cmd_out, rp);
		if (cmd_out[0] == "NOTICE")
			this->NOTICEcmd(cmd_out, rp);
		if (cmd_out[0] == "JOIN")
			this->JOINcmd(cmd_out, rp);
		if (cmd_out[0] == "MODE")
			this->MODEcmd(cmd_out, rp);
		if (cmd_out[0] == "KICK")
			this->KICKcmd(cmd_out, rp);
		if (cmd_out[0] == "INVITE")
			this->INVITEcmd(cmd_out, rp);
		if (cmd_out[0] == "PART")
			this->PARTcmd(cmd_out, rp);
	}
	else
	{
		this->users_DB[rp]->pending_msgs.push_back("451 ERR_NOTREGISTERED :You have not registered\n");
	}
}

void	Server::name_later(int p_i)
{
	std::string cmd;
	int rp = (p_i - 1) / 2;
	if (!read_sock(users_DB[rp]->sockfd, cmd))
		this->disconnect_client(rp);
	else
		this->cmd_manager(cmd, rp);
}

void	Server::accept_connection()
{
	int newsockfd;
	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);

	if ((newsockfd = accept(this->sockfd, (struct sockaddr *)&cli_addr, &clilen)) == -1)
	{
		error_lol("Error: Accepting failure");
		exit(EXIT_FAILURE);
	}
	this->users_DB.push_back(this->new_client(cli_addr, newsockfd));
	this->poll_socket.push_back(this->users_DB.back()->sock_pollin);  // unpair
	this->poll_socket.push_back(this->users_DB.back()->sock_pollout); // pair
}

Client*	Server::new_client(struct sockaddr_in cli_addr, int newsockfd)
{
	return (new Client(cli_addr, newsockfd));
}

void	Server::setup_server(char **args)
{
	this->port = std::stoi(args[0]);
	this->password = args[1];
	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;
	this->serv_addr.sin_port = htons(port);
}

Server::Server()
{
}

Server::~Server()
{
	std::cout << "Server Shuted down!" << std::endl;
	for (size_t i = 0; i < this->users_DB.size(); i++)
		delete this->users_DB[i];
	this->users_DB.clear();
	close(this->sockfd);
}

void	Server::init_Server()
{
	if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error_lol("Error: Socket failure\n");
	if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		error_lol("Error: Bind failure");
	if (listen(sockfd, 5) < 0)
		error_lol("Error: Listen failure\n");
	this->poll_sockfd.fd = this->sockfd;
	this->poll_sockfd.events = POLLIN;
	this->poll_socket.push_back(poll_sockfd);
}

void	Server::start_connection()
{
	while (1)
	{
		for (int p = this->poll_socket.size() - 1; p >= 0; p--)
		{
			int t;
			if ((t = poll(&(this->poll_socket[p]), 1, 0)))
			{
				if (t == -1)
					error_lol("Error: poll failed\n");
				if (!p)
					this->accept_connection();
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
