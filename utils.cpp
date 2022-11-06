#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

std::string	read_sock(int newsockfd)
{
	int i;
    char buffer[1000];
	std::string msg;

	while ((i = recv(newsockfd, buffer,1000, 0)))
	{
		if (i == -1)	{
			perror("Error: In Reading Socket");
			exit(EXIT_FAILURE);
		}
		msg += buffer;
		if (std::strchr(buffer, '\n'))
			break;
		bzero(buffer, 1000);
	}
	return (msg);
}
