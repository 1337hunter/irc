# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: salec <salec@student.21-school.ru>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/10 22:22:12 by salec             #+#    #+#              #
#    Updated: 2020/10/26 17:37:43 by salec            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ircserv
SRC			= main.cpp ircserv.cpp client.cpp channel.cpp error_handle.cpp
SRCDIR		= ./srcs/
SRC			:= $(addprefix $(SRCDIR), $(SRC))
OBJ			= $(SRC:.cpp=.o)
INCLUDEDIR	= ./includes/
HEADERS		= ircserv.hpp client.hpp channel.hpp error_handle.hpp \
			reply_codes.hpp error_codes.hpp common_defines.hpp
HEADERS		:= $(addprefix $(INCLUDEDIR), $(HEADERS))

CC			= clang++
CFLAGS		= -g -Wall -Wextra -Werror -I$(INCLUDEDIR)
EXECFLAGS	= $(CFLAGS)

SHELL		= /bin/zsh
RED			= \e[31m
GREEN		= \e[32m
CYAN		= \e[36m
NC			= \e[0m
ULINE		= "\e[4m"
ULINEF		= "\e[24m"

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
