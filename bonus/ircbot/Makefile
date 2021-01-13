# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: salec <salec@student.21-school.ru>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/10 22:22:12 by salec             #+#    #+#              #
#    Updated: 2021/01/13 21:42:50 by salec            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ircbot
SRC			= main.cpp ircbot.cpp jsonparser.cpp tools.cpp cmds.cpp \
			cmds/cmd_help.cpp cmds/cmd_info.cpp cmds/cmd_version.cpp \
			cmds/cmd_weather.cpp
OBJ			= $(SRC:.cpp=.o)
SRCDIR		= ./srcs/
OBJDIR		= ./objs/
SRCFILES	= $(addprefix $(SRCDIR), $(SRC))
OBJFILES	= $(addprefix $(OBJDIR), $(OBJ))
INCLUDEDIR	= ./includes/
HEADERS		= ircbot.hpp common.hpp tools.hpp cmds.hpp \
			jsonparser.hpp weather.hpp
HEADERS		:= $(addprefix $(INCLUDEDIR), $(HEADERS))

CC = clang++
CFLAGS = -Wall -Wextra -Werror -I$(INCLUDEDIR) -std=c++98 -g
EXECFLAGS = $(CFLAGS)

.PHONY: all bonus clean fclean re

all: $(NAME)

$(NAME): $(OBJDIR) $(OBJFILES)
	$(CC) -o $@ $(OBJFILES) $(EXECFLAGS)

$(OBJDIR)%.o: $(SRCDIR)%.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR) $(OBJDIR)cmds/

clean:
	@/bin/rm -rf $(OBJDIR)

fclean: clean
	@/bin/rm -f $(NAME)

re: fclean all
