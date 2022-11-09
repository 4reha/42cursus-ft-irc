SRC		= main.cpp utils.cpp #...
HEADERS	= Server.hpp Client.hpp

NAME	= ./ircserv

CC = c++
CFLAGS = -Wall -Werror -Wextra

RM = /bin/rm -f

all: $(NAME)

run		: $(NAME)
	$(NAME) 6667 passcode

$(NAME): $(HEADERS) $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(NAME)

clean:
	$(RM) $(NAME)

fclean: clean

re: fclean all

.PHONY: all clean fclean re