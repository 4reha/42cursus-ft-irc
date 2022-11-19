#pragma once
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <poll.h>
#include <arpa/inet.h>
#include <cmath>
#include <fstream>
#include <map>
#include <vector>

#include "Client.hpp"
#include "Bot.hpp"
#include "Channel.hpp"


int 						read_sock(int newsockfd, std::string &cmd);
bool						ft_isvalidnick(const std::string& str);
bool						is_channel_prefix(int c);
bool 						isModeChar(int c);
bool 						ft_isspeacial(int c);
void 						error_lol(std::string msg);
std::string					str_join(std::vector<std::string>& v, size_t i);
std::vector<std::string> 	ft_csplit(const std::string& str, std::string c);


class Server
{
	private:
		int 								port;
		int 								sockfd;
		time_t 								UpTime;
		std::string 						password;
		struct pollfd 						poll_sockfd;
		struct sockaddr_in 					serv_addr;
		std::vector<Client *> 				users_DB;
		std::map<std::string, Channel *> 	channel_DB;
		std::vector<struct pollfd> 			poll_socket;

		std::fstream 						history;
		Bot									botChat;

		int			search_client		(std::string nickname);

		void		accept_connection	();
		void		sockRead			(int p_i);
		void		register_newClient	(int ui);
		void		deny_newClient		(int ui, int n);
		void		cmd_manager			(std::string cmd, int rp);

		void		PASScmd				(std::vector<std::string> &pass_cmd, int ui);
		void		NICKcmd				(std::vector<std::string> &pass_cmd, int ui);
		void		USERcmd				(std::vector<std::string> &pass_cmd, int ui);	
		void		PRIVMSGcmd			(std::vector<std::string> &pass_cmd, int ui);
		void		NOTICEcmd			(std::vector<std::string> &pass_cmd, int ui);
		void		JOINcmd				(std::vector<std::string> &pass_cmd, int ui);
		void		MODEcmd				(std::vector<std::string> &pass_cmd, int ui);
		void		TOPICcmd			(std::vector<std::string> &pass_cmd, int ui);
		void		KICKcmd				(std::vector<std::string> &pass_cmd, int ui);
		void		INVITEcmd			(std::vector<std::string> &pass_cmd, int ui);
		void		PARTcmd				(std::vector<std::string> &pass_cmd, int ui);

		void		disconnect_client	(int ui);
		void		welcome_client		(int ui);
		Client*		new_client			(struct sockaddr_in cli_addr, int newsockfd);

		bool		is_duplicated		(std::string nickname);
		void		ChannelCreate		(Channel* channel, Client* user, std::string key);
		void		ChannelJoin		(Channel* channel, Client* user, std::string key);
		void		channel_msg			(std::map<std::string, Channel*>::iterator it, int ui, std::string msg);


	public:
		Server(char **args);
		~Server();
		void	setup_server(char ** args);
		void 	init_Server();
		void 	start_connection();
};