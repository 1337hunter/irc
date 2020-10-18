# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: salec <salec@student.21-school.ru>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/10 22:22:12 by salec             #+#    #+#              #
#    Updated: 2020/10/18 20:50:58 by salec            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRC = main.cpp error_handle.cpp

SHELL = /bin/zsh
SRCDIR = ./srcs/
SRC := $(addprefix $(SRCDIR), $(SRC))
OBJ = $(SRC:.cpp=.o)
INCLUDEDIR = ./includes/
HEADERS = ircserv.hpp error_handle.hpp
HEADERS := $(addprefix $(INCLUDEDIR), $(HEADERS))

CC = clang++
CFLAGS = -Wall -Wextra -Werror -I$(INCLUDEDIR)
EXECFLAGS = $(CFLAGS)

RED = \e[31m
GREEN = \e[32m
CYAN = \e[36m
NC = \e[0m
ULINE = "\e[4m"
ULINEF = "\e[24m"

.PHONY: all bonus clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	@echo "linking $(GREEN)$(NAME)$(NC)"
	@$(CC) -o $@ $^ $(EXECFLAGS)
	@echo "$(CYAN)executable is ready$(NC)"

%.o: %.cpp $(HEADERS)
	@echo "compiling" $(ULINE)$<$(ULINEF)
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(RED)Cleaning object files$(NC)"
	@/bin/rm -f $(OBJ)

fclean: clean
	@echo "$(RED)Deleting $(NAME) executable$(NC)"
	@/bin/rm -f $(NAME)

re: fclean all
