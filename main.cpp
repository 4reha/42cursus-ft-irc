#include "Server.hpp"
#include "Clients.hpp"


void error_lol(std::string msg)
{
    std::cerr << msg << std::endl;
    exit(1);
}

bool	isAllNum(std::string str)
{
	for (size_t i = 0; i < str.size(); i++)
		if (! std::isdigit(str[i]))
			return (false);
	return (true);
}

int main(int ac, char** av)
{
    if (ac != 3)
        error_lol("Usage error: ./ircserv <port> <password>");

	if (!isAllNum(av[1]))
		error_lol("Error: Port: Not A Number!");

	Server servObj(&av[1]);
	servObj.init_Server();
	servObj.start_connection();
	return (EXIT_SUCCESS);
}