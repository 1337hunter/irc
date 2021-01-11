# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: salec <salec@student.21-school.ru>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/10 22:22:12 by salec             #+#    #+#              #
#    Updated: 2021/01/09 19:03:40 by salec            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircbot
SRC = ircbot.cpp tools.cpp
OBJ = $(SRC:.cpp=.o)
INCLUDEDIR = ./
HEADERS = $(INCLUDEDIR)ircbot.hpp
CC = clang++
CFLAGS = -Wall -Wextra -Werror -I$(INCLUDEDIR) -std=c++98 -g
EXECFLAGS = $(CFLAGS)

.PHONY: all bonus clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $@ $^ $(EXECFLAGS)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@/bin/rm -f $(OBJ)

fclean: clean
	@/bin/rm -f $(NAME)

re: fclean all
