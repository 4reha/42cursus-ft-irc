#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int read_sock(int newsockfd, std::string &cmd)
{
	int i = 0;
	int j = 0;
    char buffer = 0;

	while ((i = recv(newsockfd, &buffer,1, 0)))
	{		if (i == -1)	{
			perror("Error: In Reading Socket");
			exit(EXIT_FAILURE);
		}
		j += i;
		if (buffer == '\n')
			break;
		if (buffer != '\r')
			cmd += buffer;
		buffer = 0;
	}
	return (j);
}

bool	ft_isspeacial(int c)
{
	if (c == '-' || c == '[' || c == ']' || c == '\\' || c == '`' || c == '^' || c == '{' || c == '}')
		return (true);
	return (false);
}