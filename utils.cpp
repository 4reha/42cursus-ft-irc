#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <vector>

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

bool 	isModeChar(int c)
{
	if (c == 'O' || c == 'o' || c == 'v' || c == 'a' || c == 'i' || c == 'm' \
		|| c == 'n' || c == 'q' || c == 'p' || c == 's' || c == 'r' || c == 't' \
		|| c == 'k' || c == 'l' || c == 'b' || c == 'e' || c == 'I')
		return (true);
	return (false);
}

bool	is_channel_prefix(int c)
{
	if (c == '+' || c == '&' || c == '!' || c == '#')
		return (true);
	return (false);
}

std::string	set_strwidth(const std::string& str)
{
	int i = 24 - str.length();
	std::string tmp(str);
	while (i--)
		tmp += " ";
	return (tmp);
}

std::string	str_join(std::vector<std::string>& v, size_t i)
{
	std::string tmp;

	tmp = v[i++];
	for (size_t n = i; n < v.size(); n++)
		tmp += " " + v[n];
	return (tmp);
}

bool	ft_isvalidnick(const std::string& str)
{
	for (size_t i = 0; i < str.size(); i++)
		if (!std::isalnum(str[i]) && !ft_isspeacial(str[i]))
			return (false);
	return (true);
}

std::vector<std::string> ft_csplit(const std::string& str, std::string c)
{
	std::vector<std::string> splited;
	for (char *token = std::strtok(const_cast<char *>(str.c_str()), c.c_str()); token != NULL; token = std::strtok(nullptr, c.c_str()))
		splited.push_back(token);
	return (splited);
}