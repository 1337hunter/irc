NAME		= ircserv
SRC			= main.cpp ircserv.cpp ircsock_base.cpp ircsock_tls.cpp \
			tools.cpp stringtools.cpp timetools.cpp error_handle.cpp \
			message.cpp parse.cpp commands.cpp cmds/cmd_tools.cpp \
			client.cpp channel.cpp \
			cmds/cmd_connect.cpp cmds/cmd_nick.cpp cmds/cmd_pass.cpp \
			cmds/cmd_user.cpp cmds/cmd_ping.cpp cmds/cmd_quit.cpp \
			cmds/cmd_server.cpp cmds/cmd_squit.cpp cmds/cmd_oper.cpp \
			cmds/cmd_error.cpp cmds/cmd_admin.cpp cmds/cmd_pong.cpp \
			cmds/cmd_motd.cpp cmds/cmd_userhost.cpp cmds/cmd_version.cpp \
			cmds/cmd_info.cpp cmds/cmd_time.cpp cmds/cmd_names.cpp \
			cmds/cmd_join.cpp cmds/cmd_privmsg.cpp cmds/cmd_invite.cpp \
			cmds/cmd_who.cpp cmds/cmd_whois.cpp cmds/cmd_whowas.cpp \
			cmds/cmd_part.cpp cmds/cmd_mode.cpp cmds/cmd_notice.cpp \
			cmds/cmd_away.cpp cmds/cmd_kill.cpp cmds/cmd_njoin.cpp \
			cmds/cmd_lusers.cpp cmds/cmd_links.cpp cmds/cmd_stats.cpp \
			cmds/cmd_ison.cpp cmds/cmd_users.cpp cmds/cmd_topic.cpp \
			cmds/cmd_kick.cpp cmds/cmd_trace.cpp cmds/cmd_service.cpp \
			cmds/cmd_die.cpp cmds/cmd_list.cpp cmds/cmd_servlist.cpp \
			cmds/cmd_wallops.cpp cmds/cmd_rehash.cpp cmds/cmd_squery.cpp

OBJ			= $(SRC:.cpp=.o)
SRCDIR		= ./srcs/
OBJDIR		= ./objs/
SRCFILES	= $(addprefix $(SRCDIR), $(SRC))
OBJFILES	= $(addprefix $(OBJDIR), $(OBJ))
DEPFILES	= $(OBJFILES:.o=.d)
INCLUDEDIR	= ./includes/
HEADERS		= ircserv.hpp tools.hpp error_handle.hpp \
			reply_codes.hpp error_codes.hpp common_defines.hpp \
			client.hpp channel.hpp message.hpp commands.hpp
HEADERS		:= $(addprefix $(INCLUDEDIR), $(HEADERS))
PWD			:= $(shell pwd)

SSLSRCDIR	= ./openssl-1.1.1h/
SSLDIR		= ./openssl/
SSLINCLUDE	= $(SSLDIR)include/
SSLLIBDIR	= $(SSLDIR)lib/
SSLLIBS		= $(SSLLIBDIR)libssl.a $(SSLLIBDIR)libcrypto.a
# openssl config flags to compile static only: no-shared
SSLFLAG		= --prefix=$(PWD)/openssl --openssldir=$(PWD)/openssl \
			no-shared no-threads
TLSCERT		= ./conf/$(NAME).crt ./conf/$(NAME).key

CC			= clang++
CFLAGS		= -Wall -Wextra -Werror -I$(INCLUDEDIR) -I$(SSLINCLUDE) -MMD
CFLAGS		+= -DFD_MAX=1024 -DBUF_SIZE=512 -DWHOWAS_MAX=2000
CFLAGS		+= -DPING_TIMEOUT=60 -DPING_FREQUENCY=60
CFLAGS		+= -g -DDEBUG_MODE=1 -fsanitize=address
# linux openssl requires libdl and libpthread (for static lib)
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
OSNAME += $(YELLOW)(debug mode)$(NC)

# just in case openssl is not installed on system somehow
ifeq (, $(shell which openssl))
	OPENSSL = $(SSLDIR)bin/openssl
else
	OPENSSL	= openssl
endif

RED			= \e[31m
GREEN		= \e[32m
YELLOW		= \e[93m
CYAN		= \e[36m
NC			= \e[0m
ULINE		= \e[4m
ULINEF		= \e[24m

.PHONY: all bonus debugmsg openssl delssl gencert delcert clean fclean re

all: debugmsg $(NAME)

bonus: $(NAME)
	@make -C ./bot

-include $(DEPFILES)

$(NAME): $(SSLLIBS) $(OBJDIR) $(OBJFILES)
	@echo "linking $(GREEN)$@$(NC) for $(OSNAME)"
	@$(CC) -o $@ $(OBJFILES) $(EXECFLAGS)
	@echo "$(CYAN)executable is ready$(NC)"
	@echo "-------------------------------------------------"
	@echo "Now you can run 'make install' to install ircserv"
	@echo "-------------------------------------------------"

$(OBJDIR)%.o: $(SRCDIR)%.cpp
	@echo "compiling $(ULINE)$<$(ULINEF)"
	@$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR):
	@mkdir -p $(OBJDIR) $(OBJDIR)cmds/

# c++98 compatibility
CFLAGS += -std=c++98 -DSTD_CPP98
OSNAME += (C++98 release)

# debugging rules
debugmsg:
	@echo "$(YELLOW)compiling $(NAME) in debug mode$(NC)"

openssl: $(SSLLIBS)

delssl:
	@echo "$(RED)Deleting $(SSLSRCDIR)$(NC)"
	@/bin/rm -rf $(SSLSRCDIR)
	@echo "$(RED)Deleting $(SSLDIR)$(NC)"
	@/bin/rm -rf $(SSLDIR)

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

delcert:
	@echo "$(RED)Deleting certificates $(TLSCERT)$(NC)"
	@/bin/rm -f $(TLSCERT)

$(TLSCERT):
	@echo "generating tls certificate..."
	@$(OPENSSL) req \
		-x509 -nodes -days 365 -newkey rsa:4096 \
		-keyout $(word 2,$(TLSCERT)) \
		-out $(word 1,$(TLSCERT)) \
		-subj "/C=RU/ST=Moscow/L=Moscow/O=42/OU=21/CN=ircserv"
	@echo "certificate file\t$(word 1,$(TLSCERT))"
	@echo "certificate key\t\t$(word 2,$(TLSCERT))"

clean:
	@echo "$(RED)Cleaning object files$(NC)"
	@/bin/rm -rf $(OBJDIR)
	@echo "$(RED)Deleting $(SSLSRCDIR)$(NC)"
	@/bin/rm -rf $(SSLSRCDIR)

fclean: clean
	@echo "$(RED)Deleting $(NAME) executable$(NC)"
	@/bin/rm -f $(NAME)

re: fclean all

install:
	@mkdir -p /home/se/ircserv
	@mkdir -p /home/se/ircserv/conf
	@cp $(NAME) /home/se/ircserv/$(NAME)
	@cp ./conf/* /home/se/ircserv/conf
