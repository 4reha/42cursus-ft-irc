#include "Client.hpp"

Client::Client(struct sockaddr_in cli_addr, int newsockfd)
{
	this->logtime = time(0);
	this->ip_addr = inet_ntoa(cli_addr.sin_addr);
	this->port = ntohs(cli_addr.sin_port);
	this->sockfd = newsockfd;
	this->sock_pollin.fd = newsockfd;
	this->sock_pollout.fd = newsockfd;
	this->sock_pollin.events = POLLIN;
	this->sock_pollout.events = POLLOUT;
	this->Registered = false;
	this->Authenticated = false;
	this->nicked = false;
	this->named = false;
}

Client::Client(Client const &cpy)
{
	this->ip_addr = cpy.ip_addr;
	this->port = cpy.port;
	this->sockfd = cpy.sockfd;
	this->nickname = cpy.nickname;
	this->username = cpy.username;
	this->sock_pollin = cpy.sock_pollin;
	this->sock_pollout = cpy.sock_pollout;
	this->Registered = false;
	this->Authenticated = false;
	this->nicked = false;
	this->named = false;
}

Client::~Client()
{
	if (this->Registered)
		std::cout << "user " << this->nickname << " has disconnected!" << std::endl;
	close(this->sockfd);
}

bool	Client::join_channel(Channel *channel, std::string mode)
{
	this->channels.insert(std::make_pair(channel->Name, channel));
	return (channel->Members.insert(std::make_pair(this, mode)).second);
}

void 	Client::build()
{
	this->userinfo = this->nickname + "!" + this->username + "@" + this->ip_addr;
}

void	Client::leaveChannel(Channel* channel, std::string msg)
{
	channel->broadcast_msg(nullptr, msg);
	channel->remove_user(this);
}

void	Client::leaveAllChans(std::string msg)
{
	std::string format = ":" + this->userinfo + " PART ";
	while (this->channels.begin() != this->channels.end())
		this->leaveChannel(this->channels.begin()->second, format + this->channels.begin()->second->Name + msg);
	this->channels.clear();
}
