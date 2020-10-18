# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: salec <salec@student.21-school.ru>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/10 22:22:12 by salec             #+#    #+#              #
#    Updated: 2020/10/18 13:28:38 by gbright          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRC = main.cpp

SRCDIR = ./srcs/
SRC := $(addprefix $(SRCDIR), $(SRC))
OBJ = $(SRC:.cpp=.o)
INCLUDEDIR = ./includes/
HEADERS = $(INCLUDEDIR)ircserv.hpp

CC = clang++
CFLAGS = -Wall -Wextra -Werror -I$(INCLUDEDIR)
EXECFLAGS = $(CFLAGS)

.PHONY: all bonus clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	@echo "linking"
	@$(CC) -o $@ $^ $(EXECFLAGS)
	@echo "\e[36mexecutable is ready"

%.o: %.cpp $(HEADERS)
	@echo "compiling" "\e[4m" $< "\e[24m"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@/bin/rm -f $(OBJ)

fclean: clean
	@/bin/rm -f $(NAME)

re: fclean all
