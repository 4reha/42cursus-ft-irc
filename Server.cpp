#include "Server.hpp"

Server::Server(char **args)
{
	this->UpTime = time(0);
	this->port = std::stoi(args[0]);
	this->password = args[1];
	this->serv_addr.sin_family = AF_INET;
	this->serv_addr.sin_addr.s_addr = INADDR_ANY;
	this->serv_addr.sin_port = htons(port);
	this->opt = 1;
	this->history.open("History_DB.log", std::ios::in | std::ios::out | std::ios::app);
	this->history << "--------------------------------------------------------------------------------" << std::endl;
	this->history << "--------------------------------------------------------------------------------" << std::endl;
	std::cout << "IRC Server UP!" << std::endl;
	this->history << "IRC Server UP!" << std::endl;
}

Server::~Server()
{
	for (size_t i = 0; i < this->users_DB.size(); i++)
		delete this->users_DB[i];
	this->users_DB.clear();
	std::cout << "Server Shuted down!" << std::endl;
	this->history << "Server Shuted down!" << std::endl;
	this->history.close();
	close(this->sockfd);
}

void	Server::welcome_client(int ui)
{
	this->users_DB[ui]->Registered = true;
	this->users_DB[ui]->build();
	std::cout << "New Client has connected from " << this->users_DB[ui]->ip_addr;
	this->history << "New Client has connected from " << this->users_DB[ui]->ip_addr;
	std::cout << ":" << this->users_DB[ui]->port << " as: " << this->users_DB[ui]->nickname;
	this->history << ":" << this->users_DB[ui]->port << " as: " << this->users_DB[ui]->nickname;
	std::cout << " id: " << ui << std::endl;
	this->history << " id: " << ui << std::endl;
	this->users_DB[ui]->pending_msgs.push_back("001 " + this->users_DB[ui]->nickname + " :Welcome to the Internet Relay Network " + this->users_DB[ui]->userinfo + "\n");
	this->users_DB[ui]->pending_msgs.push_back("002 " + this->users_DB[ui]->nickname + " :Your host is e-LEMON-tor, running version 1.12\n");
	this->users_DB[ui]->pending_msgs.push_back("003 " + this->users_DB[ui]->nickname + " :This server was created " + ctime(&this->UpTime));
	this->users_DB[ui]->pending_msgs.push_back("004 " + this->users_DB[ui]->nickname + " :e-LEMON-ator 1.12 0 *\n");
}

void	Server::deny_newClient(int ui, int n)
{
	std::cout << "Connection refused from: " << users_DB[ui]->ip_addr;
	this->history << "Connection refused from: " << users_DB[ui]->ip_addr;
	std::cout << ":" << users_DB[ui]->port << ", Using Password: ";
	this->history << ":" << users_DB[ui]->port << ", Using Password: ";
	if (n < 2)
	{
		std::cout << "NO!" << std::endl;
		this->history << "NO!" << std::endl;
		users_DB[ui]->pending_msgs.push_back("461 ERR_NEEDMOREPARAMS <PASS> :Not enough parameters\n");
	}
	else
	{
		std::cout << "YES!" << std::endl;
		this->history << "YES!" << std::endl;
		users_DB[ui]->pending_msgs.push_back("464 ERR_PASSWDMISMATCH :Password incorrect\n");
	}
}

int		Server::search_client(std::string nickname)
{
	for (size_t i = 0; i < this->users_DB.size(); i++)
		if (nickname == this->users_DB[i]->nickname)
			return (i);
	return (-1);
}

void	Server::cmd_manager(std::string cmd, int rp)
{
	std::cout << "Form "<< this->users_DB[rp]->nickname << ": " << cmd << std::endl;
	this->history << "Form "<< this->users_DB[rp]->nickname << ": " << cmd << std::endl;
	std::string cpy = cmd;
	std::vector<std::string> cmd_out;
	for (char *token = std::strtok(const_cast<char *>(cmd.c_str()), " "); token != NULL; token = std::strtok(nullptr, " "))
		cmd_out.push_back(token);
	if (!cmd_out.size())
		return ;
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
		this->QUITcmd(cmd_out, rp);
	else if (this->users_DB[rp]->Registered)
	{
		if (cmd_out[0] == "PRIVMSG")
			this->PRIVMSGcmd(cmd_out, rp);
		else if (cmd_out[0] == "NOTICE")
			this->NOTICEcmd(cmd_out, rp);
		else if (cmd_out[0] == "JOIN")
			this->JOINcmd(cmd_out, rp);
		else if (cmd_out[0] == "MODE")
			this->MODEcmd(cmd_out, rp);
		else if (cmd_out[0] == "KICK")
			this->KICKcmd(cmd_out, rp);
		else if (cmd_out[0] == "INVITE")
			this->INVITEcmd(cmd_out, rp);
		else if (cmd_out[0] == "PART")
			this->PARTcmd(cmd_out, rp);
		else if (cmd_out[0] == "TOPIC")
			this->TOPICcmd(cmd_out, rp);
		else if (cmd_out[0] == "BOT")
			this->botChat.botcmd(this->users_DB, cmd_out, rp);
		else
			this->users_DB[rp]->pending_msgs.push_back("421 ERR_UNKNOWNCOMMAND " + cmd_out[0] + " :Unknown command\n");
	}
	else
	{
		this->users_DB[rp]->pending_msgs.push_back("451 ERR_NOTREGISTERED :You have not registered\n");
	}
}

void	Server::sockRead(int p_i)
{
	std::string cmd;
	std::vector<std::string> cmd_out;
	int rp = (p_i - 1) / 2;
	if (!read_sock(users_DB[rp]->sockfd, cmd))	{
		cmd_out.push_back("QUIT");
		cmd_out.push_back(this->users_DB[rp]->nickname);
		this->QUITcmd(cmd_out, rp);
	}
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

void	Server::init_Server()
{
	if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		error_lol("Error: Socket failure\n");
	if (setsockopt(this->sockfd, SOL_SOCKET, SO_REUSEADDR, &this->opt, sizeof(this->opt) ) < 0)
		error_lol("Error: Socket Option failure\n");
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
					this->sockRead(p);
				}
				else if (!(p % 2))
				{ // if pair
					int rp = (p - 2) / 2;
					while (users_DB[rp]->pending_msgs.size())
					{
						std::cout << "To " << this->users_DB[rp]->nickname << ": " <<  users_DB[rp]->pending_msgs[0] << std::endl;
						this->history << "To " << this->users_DB[rp]->nickname << ": " <<  users_DB[rp]->pending_msgs[0]<< std::endl;
						send(users_DB[rp]->sockfd, users_DB[rp]->pending_msgs[0].c_str(), users_DB[rp]->pending_msgs[0].length(), 0);
						users_DB[rp]->pending_msgs.erase(users_DB[rp]->pending_msgs.begin());
					}
				}
			}
		}
	}
}

Client*	Server::new_client(struct sockaddr_in cli_addr, int newsockfd)
{
	return (new Client(cli_addr, newsockfd));
}

bool	Server::is_duplicated(std::string nickname)
{
	for (size_t i = 0; i < this->users_DB.size(); i++)
		if (nickname == this->users_DB[i]->nickname)
			return (true);
	return (false);
}

void    Server::channel_msg(std::map<std::string, Channel*>::iterator it, int ui, std::string msg)
{
	std::string format = ":" + this->users_DB[ui]->userinfo + " PRIVMSG ";
	if (it->second->isBanned(this->users_DB[ui]))
        this->users_DB[ui]->pending_msgs.push_back("404 ERR_CANNOTSENDTOCHAN " + it->second->Name + " :Cannot send to channel (+b)\n");
	else if (it->second->isMode('m'))
	{
		if (it->second->UserIsV(this->users_DB[ui]) || it->second->isOperator(this->users_DB[ui]))
			it->second->broadcast_msg(this->users_DB[ui], format + it->second->Name + msg + "\n");
		else
			this->users_DB[ui]->pending_msgs.push_back("404 ERR_CANNOTSENDTOCHAN " + it->second->Name + " :Cannot send to channel (+m)\n");
	}
	else {
		if (it->second->isMode('n'))	{
			if (it->second->isMember(this->users_DB[ui]))
				it->second->broadcast_msg(this->users_DB[ui], format + it->second->Name + msg + "\n");
			else
				this->users_DB[ui]->pending_msgs.push_back("404 ERR_CANNOTSENDTOCHAN " + it->second->Name + " :Cannot send to channel (+n)\n");
		}
		else
			it->second->broadcast_msg(this->users_DB[ui], format + it->second->Name + msg + "\n");
	}
    
}

void	Server::ChannelCreate(Channel* channel, Client* user, std::string key)
{
	std::string format = ":" + user->userinfo+ " JOIN ";
	if (user->channels.size() == MAXCHAN)
		user->pending_msgs.push_back("405 ERR_TOOMANYCHANNELS " + channel->Name + " :You have joined too many channels\n");
	else	{
		user->join_channel(channel, "Oo");
		if (key != "")
			channel->setModes("+k");
		channel->Password = key;
		channel->broadcast_msg(nullptr, format + channel->Name + "\n");
		user->pending_msgs.push_back("353 " + user->nickname + " = " + channel->Name + channel->getUsers() + "\n");
		user->pending_msgs.push_back("366 " + user->nickname + " " + channel->Name + " :End of /NAMES list\n");
	}
}

void	Server::ChannelJoin(Channel* channel, Client* user, std::string key)
{
	std::string format = ":" + user->userinfo+ " JOIN ";
	if (channel->isInvitOnly() && !channel->isinvited(user))
		user->pending_msgs.push_back("473 ERR_INVITEONLYCHAN " + channel->Name + " :Cannot join channel (+i)\n");
	else if (channel->isBanned(user))
		user->pending_msgs.push_back("474 ERR_BANNEDFROMCHAN " + channel->Name + " :Cannot join channel (+b)\n");
	else if (channel->isFull())
		user->pending_msgs.push_back("471 ERR_CHANNELISFULL " + channel->Name + " :Cannot join channel (+l)\n");
	else if (user->channels.size() == MAXCHAN)
		user->pending_msgs.push_back("405 ERR_TOOMANYCHANNELS " + channel->Name + " :You have joined too many channels\n");
	else if ((channel->isMode('k') && channel->Password == key ) || !channel->isMode('k'))	{
		if (user->join_channel(channel, ""))	{
			channel->broadcast_msg(nullptr, format + channel->Name + "\n");
			user->pending_msgs.push_back("353 " + user->nickname + " = " + channel->Name + channel->getUsers() + "\n");
			user->pending_msgs.push_back("366 " + user->nickname + " " + channel->Name + " :End of /NAMES list\n");
		}
	}
	else
		user->pending_msgs.push_back("475 ERR_BADCHANNELKEY " + channel->Name + " :Cannot join channel (+k)\n");

}