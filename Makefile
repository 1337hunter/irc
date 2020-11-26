# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: salec <salec@student.21-school.ru>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/05/10 22:22:12 by salec             #+#    #+#              #
#    Updated: 2020/11/26 19:39:24 by salec            ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ircserv
SRC			= main.cpp ircserv.cpp ircsock_base.cpp ircsock_tls.cpp \
			tools.cpp error_handle.cpp message.cpp parse.cpp \
			client.cpp channel.cpp \
			cmds/cmd_connect.cpp cmds/cmd_nick.cpp cmds/cmd_pass.cpp \
			cmds/cmd_user.cpp cmds/cmd_ping.cpp cmds/cmd_quit.cpp \
			cmds/cmd_server.cpp cmds/cmd_squit.cpp cmds/cmd_oper.cpp \
			cmds/cmd_tools.cpp cmds/cmd_error.cpp cmds/cmd_admin.cpp \
			cmds/cmd_motd.cpp cmds/cmd_userhost.cpp cmds/cmd_who.cpp
SRCDIR		= ./srcs/
SRC			:= $(addprefix $(SRCDIR), $(SRC))
OBJ			= $(SRC:.cpp=.o)
INCLUDEDIR	= ./includes/
HEADERS		= ircserv.hpp tools.hpp error_handle.hpp \
			reply_codes.hpp error_codes.hpp common_defines.hpp \
			client.hpp channel.hpp message.hpp tools.hpp \
			commands.hpp
HEADERS		:= $(addprefix $(INCLUDEDIR), $(HEADERS))
PWD			:= $(shell pwd)

SSLSRCDIR	= ./openssl-1.1.1h/
SSLDIR		= ./openssl/
SSLINCLUDE	= $(SSLDIR)include/
SSLLIBDIR	= $(SSLDIR)lib/
SSLLIBS		= $(SSLLIBDIR)libssl.a $(SSLLIBDIR)libcrypto.a
SSLFLAG		= --prefix=$(PWD)/openssl --openssldir=$(PWD)/openssl no-shared
# to compile static only add no-shared
TLSCERT		= ./conf/$(NAME).crt ./conf/$(NAME).key

CC			= clang++
CFLAGS		= -g -Wall -Wextra -Werror -I$(INCLUDEDIR) -I$(SSLINCLUDE)
# linux openssl requires libdl and libpthread (for static lib)
LIBFLAGS	= -L$(SSLLIBDIR) -lssl -lcrypto -ldl -lpthread
EXECFLAGS	= $(CFLAGS) $(LIBFLAGS)
SHELL		= /bin/zsh

UNAME		:= $(shell uname)
ifeq ($(UNAME), Darwin)
OSNAME		= Darwin
SSLFLAG		+= darwin64-x86_64-cc
else
	ifeq ($(UNAME), Linux)
	OSNAME	= Linux
	SSLFLAG	+= linux-x86_64-clang
	else
	OSNAME	= Unknown OS
	endif
endif

# just in case openssl is not installed on system somehow
ifeq (, $(shell which openssl))
	OPENSSL = $(SSLDIR)bin/openssl
else
	OPENSSL	= openssl
endif

RED			= \e[31m
GREEN		= \e[32m
CYAN		= \e[36m
NC			= \e[0m
ULINE		= \e[4m
ULINEF		= \e[24m

.PHONY: all bonus openssl delssl gencert clean fclean re

all: $(NAME)

$(NAME): $(SSLLIBS) $(OBJ)
	@echo "linking $(GREEN)$(NAME)$(NC) for $(OSNAME)"
	@$(CC) -o $@ $(OBJ) $(EXECFLAGS)
	@echo "$(CYAN)executable is ready$(NC)"

%.o: %.cpp $(HEADERS)
	@echo "compiling $(ULINE)$<$(ULINEF)"
	@$(CC) $(CFLAGS) -c $< -o $@

openssl: $(SSLLIBS)

$(SSLLIBS):
	@echo "building $(ULINE)OpenSSL library v1.1.1h$(ULINEF) (this will take a while)"
	@echo -n "extracting archive..."
	@tar -xf ./openssl-1.1.1h.tar.gz
	@echo "\t\tdone"
	@echo -n "configuring makefile..."
	@cd $(SSLSRCDIR) && ./Configure $(SSLFLAG) > /dev/null && cd ..
	@echo "\t\tdone"
	@echo "configured with $(SSLFLAG) params"
	@echo -n "building libraries..."
	@make -C $(SSLSRCDIR) > /dev/null 2> /dev/null
	@echo "\t\tdone"
	@echo -n "installing to ./openssl/..."
	@make -C $(SSLSRCDIR) install > /dev/null 2> /dev/null
	@echo "\tdone"

gencert: $(TLSCERT)

$(TLSCERT):
	@echo "generating tls certificate..."
	@$(OPENSSL) req \
		-x509 -nodes -days 365 -newkey rsa:4096 \
		-keyout $(word 2,$(TLSCERT)) \
		-out $(word 1,$(TLSCERT)) \
		-subj "/C=RU/ST=Moscow/L=Moscow/O=42/OU=21/CN=ircserv"
	@echo "certificate file\t$(word 1,$(TLSCERT))"
	@echo "certificate key\t\t$(word 2,$(TLSCERT))"

delssl:
	@echo "$(RED)Deleting certificates $(TLSCERT)$(NC)"
	@/bin/rm -f $(TLSCERT)
	@echo "$(RED)Deleting $(SSLDIR)$(NC)"
	@/bin/rm -rf $(SSLDIR)

clean:
	@echo "$(RED)Cleaning object files$(NC)"
	@/bin/rm -f $(OBJ)
	@echo "$(RED)Deleting $(SSLSRCDIR)$(NC)"
	@/bin/rm -rf $(SSLSRCDIR)

fclean: clean
	@echo "$(RED)Deleting $(NAME) executable$(NC)"
	@/bin/rm -f $(NAME)

re: fclean all
