#include "Server.hpp"
#include "Client.hpp"

void error_lol(std::string msg)
{
	std::cerr << msg << std::endl;
	exit(1);
}

bool isAllNum(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
		if (!std::isdigit(str[i]))
			return (false);
	return (true);
}

Server *servObj;

void	Handler(int)
{
	servObj->~Server();
	exit(0);
}

int main(int ac, char **av)
{
	if (ac != 3)
		error_lol("Usage error: ./ircserv <port> <password>");
	if (!isAllNum(av[1]))
		error_lol("Error: Port: Not A Number!");
	signal(SIGINT, Handler);
	servObj = new Server();
	servObj->setup_server(&av[1]);
	servObj->init_Server();
	servObj->start_connection();
	return (EXIT_SUCCESS);
}