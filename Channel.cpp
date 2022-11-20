#include "Channel.hpp"

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
	this->Topic = args[2];
	for (size_t i = 3; i < args.size(); i++)
		this->Topic += " " + args[i];
}

void	Channel::Bann(std::string user, char op)
{
	if (op == '+')	{
		this->Banned.insert(user);
		std::cout << user << " is banned" << std::endl;
		for (std::map<Client *,std::string>::iterator it = Members.begin(); it != Members.end(); it++)	{
			if (it->first->nickname == user)	{
				remove_user(it->first);
				return ;
			}
		}
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
				if (it->second.find(m[1]) == it->second.npos)
					it->second += m[1];
			}
			else	{
				std::cout << "HERE HERE HERE" << std::endl;
				size_t p;
				if ((p = it->second.find(m[1])) != it->second.npos)
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

std::string Channel::getBanned()
{
	std::string str = " :";
	for (std::set<std::string>::iterator it = Banned.begin(); it != Banned.end(); it++)	{
			str += *it + " ";
	}
	return (str);
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
	user->channels.erase(this->Name);
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

std::string Channel::getMode()
{
	std::string params = this->Modes;
	for (size_t i = 0; i < this->Modes.length(); i++)
	{
		if (this->Modes[i] == 'l')
			params += " " + std::to_string(this->maxMembers);
		if (this->Modes[i] == 'k')
			params += " " + this->Password;
	}
	return (params);
}

void	Channel::setupModes(Client* user, std::vector<std::string>& pass_cmd)
{
	size_t p = 3;
	std::string format = ":" + user->userinfo + " MODE " + pass_cmd[1] + " ";
	for (size_t i = 1; i < pass_cmd[2].length(); i++)	{
		if (!isModeChar(pass_cmd[2][i]))	{
			user->pending_msgs.push_back("472 ERR_UNKNOWNMODE " + std::string(1, pass_cmd[2][i]) + " :is unknown mode char to me for " + pass_cmd[1] + "\n");
			pass_cmd[2].erase(pass_cmd[2].begin() + i);
			continue ;
		}
		else if (pass_cmd[2][i] == 'o' || pass_cmd[2][i] == 'v')	{
			if (p < pass_cmd.size())	{
				if (!this->setMemModes(pass_cmd[2][0] + std::string(1, pass_cmd[2][i]), pass_cmd[p++]))
					user->pending_msgs.push_back("441 ERR_USERNOTINCHANNEL "+ pass_cmd[p - 1] + " :They aren't on " + pass_cmd[0] + "\n");
			}
			else
				user->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS MODE :Not enough parameters\n");
		}
		else if (pass_cmd[2][i] == 'l')	{
			if (p < pass_cmd.size() && pass_cmd[2][0] == '+')
				this->setLimit(pass_cmd[2][0], pass_cmd[p++]);
			else if (pass_cmd[2][0] == '-')
				this->setLimit(pass_cmd[2][0], "");
			else
				user->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS MODE :Not enough parameters\n");
		}
		else if (pass_cmd[2][i] == 'k')	{
			if (p < pass_cmd.size())	{
				if (pass_cmd[2][0] == '-')	{
					if (this->Password == pass_cmd[p++])	{
						this->setModes("-k");
						this->Password = "";
					}
					else
						user->pending_msgs.push_back("475 ERR_BADCHANNELKEY " + pass_cmd[1] + " :Cannot change key (-k)\n");
				}
				else	{
					if (this->Password == "")
						this->Password = pass_cmd[p++];
					else
						user->pending_msgs.push_back("467 ERR_KEYSET " + pass_cmd[1] + " :Channel key already set\n");
				}	
			}
			else
				user->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS MODE :Not enough parameters\n");
		}
		else if (pass_cmd[2][i] == 'b')	{
			if (p < pass_cmd.size())
				this->Bann(pass_cmd[p++], pass_cmd[2][0]);
			else	{
				user->pending_msgs.push_back("367 RPL_BANLIST " + this->Name + this->getBanned() + "\n");
				user->pending_msgs.push_back("368 RPL_ENDOFBANLIST " + this->Name + " :End of channel ban list\n");
			}
		}
		else	{
			this->setModes(pass_cmd[2][0] + std::string(1, pass_cmd[2][i]));
		}
		if (p > 3)
			this->broadcast_msg(nullptr, format + pass_cmd[2][0] + pass_cmd[2][i] + " " + pass_cmd[p - 1] + "\n");
		else if (pass_cmd[2][i] != 'l' && pass_cmd[2][i] != 'k' && pass_cmd[2][i] != 'v' && pass_cmd[2][i] != 'o')
			this->broadcast_msg(nullptr, format + pass_cmd[2][0] + pass_cmd[2][i] + "\n");
	}
}
