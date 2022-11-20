#include "Server.hpp"

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

void	Server::NICKcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::string nickName;
	if (pass_cmd.size() < 2 || (pass_cmd.size() == 2 && pass_cmd[1].length() == 1 && pass_cmd[1][0] == ':'))	{
		this->users_DB[ui]->pending_msgs.push_back("431 ERR_NONICKNAMEGIVEN :No nickname given\n");
		return ;
	}
	if (pass_cmd.size() > 2)
		nickName = std::string(pass_cmd[1], 1, pass_cmd[1].length() - 1) + " " + str_join(pass_cmd, 2);
	else
		nickName = pass_cmd[1];
	if (!ft_isvalidnick(nickName))
		this->users_DB[ui]->pending_msgs.push_back("432 ERR_ERRONEUSNICKNAME <" + nickName + "> :Erroneus nickname\n");
	else if (this->is_duplicated(nickName))
		this->users_DB[ui]->pending_msgs.push_back("433 ERR_NICKNAMEINUSE <" + nickName + "> :Nickname is already in use\n");
	else	{
		this->users_DB[ui]->nickname = nickName;
		this->users_DB[ui]->nicked = true;
		if (this->users_DB[ui]->nicked && this->users_DB[ui]->named && !this->users_DB[ui]->Registered)
			welcome_client(ui);
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

void	Server::PRIVMSGcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::string format = ":" + this->users_DB[ui]->userinfo + " PRIVMSG ";
	std::string msg;
	std::vector<std::string> receivers;

	if (pass_cmd.size() < 3)	{
		this->users_DB[ui]->pending_msgs.push_back("412 ERR_NOTEXTTOSEND :No text to send\n");
		return ;
	}
	receivers = ft_csplit(pass_cmd[1], ",");
	if (pass_cmd.size() == 3)
		msg = " :";
	msg += " "  + str_join(pass_cmd, 2);
	for (size_t i = 0; i < receivers.size(); i++)
	{
		int p;
		std::map<std::string, Channel*>::iterator it;
		if (!is_channel_prefix(receivers[i][0]) && (p = this->search_client(receivers[i])) >= 0)
			this->users_DB[p]->pending_msgs.push_back(format + receivers[i] + msg + "\n");
		else if (is_channel_prefix(receivers[i][0]) && (it = this->channel_DB.find(receivers[i])) != this->channel_DB.end())
			this->channel_msg(it, ui, msg);
		else
			this->users_DB[ui]->pending_msgs.push_back("401 ERR_NOSUCHNICK " + receivers[i] + " :No such nick/channel\n");
	}
}

void	Server::NOTICEcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::string format = ":" + this->users_DB[ui]->nickname + " NOTICE ";
	std::string msg;
	std::vector<std::string> receivers;

	receivers = ft_csplit(pass_cmd[1], ",");
	if (pass_cmd.size() == 3)
		msg = " :";
	msg += " " + str_join(pass_cmd, 2);
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

void	Server::JOINcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::vector<std::string> channels, keys;
	std::string format = ":" + this->users_DB[ui]->userinfo+ " JOIN ";
	if (pass_cmd.size() < 2)
	{
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS <JOIN> :Not enough parameters\n");
		return;
	}
	if (pass_cmd.size() == 2 && pass_cmd[1] == "0")	{
		this->users_DB[ui]->leaveAllChans();
		return ;
	}
	channels = ft_csplit(pass_cmd[1], ",");
	if (pass_cmd.size() > 2)
		keys = ft_csplit(pass_cmd[2], ",");
	for (size_t i = 0; i < channels.size(); ++i)
	{
		if (!is_channel_prefix(channels[i][0]))	{
			this->users_DB[ui]->pending_msgs.push_back("403 ERR_NOSUCHCHANNEL " + channels[i] + " :Unknown channel category\n");
			continue;
		}
		Channel *tmp_chn = new Channel(channels[i], "+");
		std::pair<std::map<std::string, Channel *>::iterator, bool> pair = this->channel_DB.insert(std::make_pair(channels[i], tmp_chn));
		if (pair.second)
		{
			try	{ this->ChannelCreate(pair.first->second, this->users_DB[ui], keys.at(i)); }
			catch(const std::exception& e)	{ this->ChannelCreate(pair.first->second, this->users_DB[ui], ""); }
		}
		else
		{
			try	{	this->ChannelJoin(pair.first->second, this->users_DB[ui], keys.at(i)); }
			catch(const std::exception& e)	{ this->ChannelJoin(pair.first->second, this->users_DB[ui], ""); }
			delete tmp_chn;
		}
	}
}

void	Server::MODEcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::string format = ":" + this->users_DB[ui]->userinfo + " MODE " + pass_cmd[1] + " ";
	std::map<std::string, Channel*>::iterator it;
	if (pass_cmd.size() < 2)
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS MODE :Not enough parameters\n");
	else if ((it = channel_DB.find(pass_cmd[1])) == channel_DB.end())
		this->users_DB[ui]->pending_msgs.push_back("403 ERR_NOSUCHCHANNEL " + pass_cmd[1] + " :No such channel\n");
	else if (!it->second->isMember(this->users_DB[ui]))
		this->users_DB[ui]->pending_msgs.push_back("442 ERR_NOTONCHANNEL " + pass_cmd[1] + " :You're not on that channel\n");
	else if (pass_cmd.size() == 2)
		this->users_DB[ui]->pending_msgs.push_back("324 RPL_CHANNELMODEIS " + pass_cmd[1] + " " + it->second->getMode() + "\n");
	else if (!it->second->isOperator(this->users_DB[ui]))
		this->users_DB[ui]->pending_msgs.push_back("482 ERR_CHANOPRIVSNEEDED " + pass_cmd[1] + " :You're not channel operator\n");
	else if (pass_cmd[2][0] == '+' || pass_cmd[2][0] == '-')
		it->second->setupModes(this->users_DB[ui], pass_cmd);
}

void	Server::KICKcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::string msg = " " + this->users_DB[ui]->nickname;
	if (pass_cmd.size() < 3)	{
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS TOPIC :Not enough parameters\n");
		return ;
	}
	if (pass_cmd.size() >= 4)
		msg = " " + pass_cmd[3] + " " + str_join(pass_cmd, 4);
	std::map<std::string, Channel*>::iterator it;
	std::string format = ":" + this->users_DB[ui]->userinfo + " KICK ";
	std::vector<std::string>	channels, users;
	channels = ft_csplit(pass_cmd[1], ",");
	users = ft_csplit(pass_cmd[2], ",");
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
				it->second->broadcast_msg(nullptr, format + " " + channels[i] + " " + users[j] + msg + "\n");
				it->second->remove_user(this->users_DB[l]);
			}
			else
				this->users_DB[ui]->pending_msgs.push_back("441 ERR_USERNOTINCHANNEL " + users[j] + " :They aren't on " + channels[i] + "\n");
		}
		else
			this->users_DB[ui]->pending_msgs.push_back("401 ERR_NOSUCHNICK " + users[j] + " :No such nick\n");
		if (channels.size() == 1)
			i--;
	}
}

void	Server::INVITEcmd(std::vector<std::string> &pass_cmd, int ui)
{
	int	p;
	std::map<std::string, Channel*>::iterator it;

	if (pass_cmd.size() < 3)
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS INVITE :Not enough parameters\n");
	else if ((p = this->search_client(pass_cmd[1])) < 0)
		this->users_DB[ui]->pending_msgs.push_back("401 ERR_NOSUCHNICK " + pass_cmd[1] + " :No such nick/channel\n");
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

void	Server::PARTcmd(std::vector<std::string> &pass_cmd, int ui)
{
	std::string msg = " " + this->users_DB[ui]->nickname;
	std::string format = ":" + this->users_DB[ui]->userinfo + " PART ";
	std::vector<std::string> channels;
	if (pass_cmd.size() < 2)	{
		this->users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS PART :Not enough parameters\n");
		return ;
	}
	else if (pass_cmd.size() > 2)
		msg = " " + pass_cmd[2] + " " + str_join(pass_cmd, 3);
	channels = ft_csplit(pass_cmd[1], ",");
	std::map<std::string, Channel*>::iterator it;
	for (size_t i = 0; i < channels.size(); i++)	{
		if ((it = channel_DB.find(channels[i])) == channel_DB.end())
			this->users_DB[ui]->pending_msgs.push_back("403 ERR_NOSUCHCHANNEL " + channels[i] + " :No such channel\n");
		else if (!it->second->isMember(this->users_DB[ui]))
			this->users_DB[ui]->pending_msgs.push_back("442 ERR_NOTONCHANNEL " + channels[i] + " :You're not on that channel\n");
		else	{
			it->second->broadcast_msg(nullptr, format + channels[i] + msg + "\n");
			it->second->remove_user(this->users_DB[ui]);
		}
	}
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
	else if (pass_cmd.size() > 2)
		it->second->setTopic(pass_cmd);
	else if (it->second->Topic == "")
		this->users_DB[ui]->pending_msgs.push_back("331 RPL_NOTOPIC " + pass_cmd[1] + " :No topic is set\n");
	else
		this->users_DB[ui]->pending_msgs.push_back("332 RPL_TOPIC " + pass_cmd[1] + " " + it->second->Topic + "\n");
}

void	Server::QUITcmd(int ui)
{
	this->users_DB[ui]->leaveAllChans();
	delete users_DB[ui];
	this->users_DB.erase(users_DB.begin() + ui);
	this->poll_socket.erase(poll_socket.begin() + (ui * 2 +1));
	this->poll_socket.erase(poll_socket.begin() + (ui * 2 +1));

}
