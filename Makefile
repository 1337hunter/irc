# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: salec <salec@student.21-school.ru>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/10 22:22:12 by salec             #+#    #+#              #
#    Updated: 2020/11/14 16:23:04 by salec            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ircserv
SRC			= main.cpp ircserv.cpp ircsock_base.cpp ircsock_tls.cpp \
			tools.cpp error_handle.cpp message.cpp parse.cpp \
			client.cpp channel.cpp \
			cmds/cmd_connect.cpp cmds/cmd_nick.cpp cmds/cmd_pass.cpp \
			cmds/cmd_user.cpp cmds/cmd_ping.cpp cmds/cmd_quit.cpp \
			cmds/cmd_server.cpp cmds/cmd_squit.cpp cmds/cmd_oper.cpp \
			cmds/cmd_tools.cpp cmds/cmd_error.cpp
SRCDIR		= ./srcs/
SRC			:= $(addprefix $(SRCDIR), $(SRC))
OBJ			= $(SRC:.cpp=.o)
INCLUDEDIR	= ./includes/
HEADERS		= ircserv.hpp tools.hpp error_handle.hpp \
			reply_codes.hpp error_codes.hpp common_defines.hpp \
			client.hpp channel.hpp message.hpp tools.hpp \
			commands.hpp
HEADERS		:= $(addprefix $(INCLUDEDIR), $(HEADERS))

CRYPTODIR	= ./openssl-1.1.1h/
CRYPTOLIBS	= $(CRYPTODIR)libssl.a $(CRYPTODIR)libcrypto.a
TLSCERT		= ./conf/$(NAME).crt ./conf/$(NAME).key

CC			= clang++
CFLAGS		= -g -Wall -Wextra -Werror -I$(INCLUDEDIR)
LIBFLAGS	= -lssl -lcrypto
EXECFLAGS	= $(CFLAGS) $(LIBFLAGS)
SHELL		= /bin/zsh

UNAME		:= $(shell uname)
ifeq ($(UNAME), Darwin)
OSNAME		= Darwin
else
	ifeq ($(UNAME), Linux)
	OSNAME	= Linux
	else
	OSNAME	= Unknown OS
	endif
endif

# doesn't work for some reason
ifeq (, $(shell which openssl))
	OPENSSL = $(CRYPTODIR)apps/openssl
else
	OPENSSL	= openssl
endif

RED			= \e[31m
GREEN		= \e[32m
CYAN		= \e[36m
NC			= \e[0m
ULINE		= "\e[4m"
ULINEF		= "\e[24m"

.PHONY: all bonus openssl gencert clean fclean re

all: $(NAME)

$(NAME): $(OBJ)
	@echo "linking $(GREEN)$(NAME)$(NC) for $(OSNAME)"
	@$(CC) -o $@ $^ $(EXECFLAGS)
	@echo "$(CYAN)executable is ready$(NC)"

%.o: %.cpp $(HEADERS)
	@echo "compiling" $(ULINE)$<$(ULINEF)
	@$(CC) $(CFLAGS) -c $< -o $@

openssl: $(CRYPTOLIBS) gencert

$(CRYPTOLIBS):
	@echo "OpenSSL v1.1.1h (this may take a while)"
	@echo -n "extracting archive..."
	@tar -xf openssl-1.1.1h.tar.gz
	@echo "\tdone"
	@echo -n "configuring makefile..."
	@cd $(CRYPTODIR) && ./config > /dev/null && cd ..
	@echo "\tdone"
	@echo -n "building libraries..."
	@make -C $(CRYPTODIR) > /dev/null 2> /dev/null
	@echo "\tdone"

gencert: $(TLSCERT)

$(TLSCERT):
	@echo -n "generating tls cert..."
	@$(CRYPTODIR)apps/openssl req \
		-x509 -nodes -days 365 -newkey rsa:4096 \
		-keyout $(word 2,$(TLSCERT)) \
		-out $(word 1,$(TLSCERT)) \
		-subj "/C=RU/ST=Moscow/L=Moscow/O=42/OU=21/CN=ircserv" \
		> /dev/null 2> /dev/null
	@echo "\tdone"
	@echo "cert file\t$(word 1,$(TLSCERT))"
	@echo "cert key\t$(word 2,$(TLSCERT))"

clean:
	@#@echo "$(RED)Cleaning crypto lib$(NC)"
	@#@make -C $(CRYPTODIR) clean
	@echo "$(RED)Cleaning object files$(NC)"
	@/bin/rm -f $(OBJ)

fclean: clean
	@echo "$(RED)Deleting $(NAME) executable$(NC)"
	@/bin/rm -f $(NAME)
#	@/bin/rm -f $(TLSCERT)
#	@/bin/rm -rf $(CRYPTODIR)
# disabled openssl dir deletion so we don't have to wait for it after make re

re: fclean all
