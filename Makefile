# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: salec <salec@student.21-school.ru>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/10 22:22:12 by salec             #+#    #+#              #
#    Updated: 2020/11/04 18:12:49 by salec            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ircserv
# SRC		= common files, classes, commands
SRC			= main.cpp ircserv.cpp tools.cpp error_handle.cpp \
			client.cpp channel.cpp \
			cmds/cmd_connect.cpp cmds/cmd_nick.cpp cmds/cmd_pass.cpp \
			cmds/cmd_user.cpp cmds/cmd_ping.cpp cmds/cmd_quit.cpp \
			cmds/cmd_server.cpp cmds/cmd_squit.cpp
SRCDIR		= ./srcs/
SRC			:= $(addprefix $(SRCDIR), $(SRC))
OBJ			= $(SRC:.cpp=.o)
INCLUDEDIR	= ./includes/
HEADERS		= ircserv.hpp tools.hpp error_handle.hpp \
			reply_codes.hpp error_codes.hpp common_defines.hpp \
			client.hpp channel.hpp \
			commands.hpp
HEADERS		:= $(addprefix $(INCLUDEDIR), $(HEADERS))

CC			= clang++
CFLAGS		= -g -Wall -Wextra -Werror -I$(INCLUDEDIR)
EXECFLAGS	= $(CFLAGS)
SHELL		= /bin/zsh

# -std=c++11 is required on darwin for lambda expressions to work
UNAME		:= $(shell uname)
ifeq ($(UNAME), Darwin)			# CFLAGS += -DDARWIN
OSNAME		= Darwin
else
	ifeq ($(UNAME), Linux)
	OSNAME	= Linux
	else
	OSNAME	= Unknown OS
	endif
endif

RED			= \e[31m
GREEN		= \e[32m
CYAN		= \e[36m
NC			= \e[0m
ULINE		= "\e[4m"
ULINEF		= "\e[24m"

.PHONY: all bonus clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	@echo "linking $(GREEN)$(NAME)$(NC) for $(OSNAME)"
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
