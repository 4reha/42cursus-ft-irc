#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

int read_sock(int newsockfd, std::string &cmd)
{
	int i = 0;
	int j = 0;
    char buffer;

	while ((i = recv(newsockfd, &buffer,1, 0)))
	{
		if (i == -1)	{
			perror("Error: In Reading Socket");
			exit(EXIT_FAILURE);
		}
		j += i;
		cmd += buffer;
		if (buffer == '\n')
			break;
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