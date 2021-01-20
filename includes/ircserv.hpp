/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 13:14:43 by salec             #+#    #+#             */
/*   Updated: 2021/01/20 17:06:47 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_HPP
# define IRCSERV_HPP

# include "common_defines.hpp"
# include <iostream>
# include <exception>
# include <string>
# include <vector>
# include <list>
# include <algorithm>
# ifndef STD_CPP98
#  include <unordered_map>
# else
#  include <map>
# endif

// time_t
# include <ctime>
// fcntl, select, socket, inet structs, inet
# include <fcntl.h>
# include <sys/types.h>
# include <sys/select.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
// read, write
# include <unistd.h>

// openssl
# include <openssl/ssl.h>
# include <openssl/err.h>

# include "error_handle.hpp"
# include "reply_codes.hpp"
# include "error_codes.hpp"
# include "client.hpp"
# include "channel.hpp"
# include "commands.hpp"

typedef std::vector<std::string>	t_strvect;
typedef std::list<Client>::iterator t_citer;

typedef struct		s_fd
{
	int				type;
	bool			tls;
	bool			status;     // for disconnect assign false
	bool			fatal;
	bool			blocked;
	bool			inprogress;
	std::string		rdbuf;
	std::string		wrbuf;
	std::string		pass;		// this struct parameters required for
	std::string		version;	// message PASS that can be received either
	std::string		flags;		// from client or server before SERVER or NICK
	std::string		options;	// register attempt
	std::string		hostname;
	SSL				*sslptr;
	time_t			lastactive;
	bool			awaitingpong;
//	STATS l related statistics
	std::string		linkname;
	int				sock;
	int				sentmsgs;
	int				recvmsgs;
	size_t			sentbytes;
	size_t			recvbytes;
	time_t			dtopened;
}					t_fd;

struct				t_server_intro
{
	std::string		servername; //server which said that 'behind' is new connection to him
	std::string		behind;
	std::string		info;
	std::string		token;
	int				hopcount;
};

typedef struct		s_server
{
	int				fd;
	int				hopcount;
	int				port;
	bool			autoconnect;
	std::string		token;
	std::string		servername;
	std::string		pass;
	std::string		info;
	std::list<t_server_intro>	routing;
	std::list<Client>			clients; // if client is not on THIS server
}					t_server;			 // client's fd must be setted to routong serv fd

typedef std::vector<t_server>::iterator	t_netit;

typedef struct		s_listen
{
	int				socket_fd;		// I don't think its necessery
	bool			serveronly;
	bool			tls;
	bool			ssl;
	int				port;
	std::string		ip;
}					t_listen;

typedef struct		s_admin
{
	bool			set;
	std::string		realname;
	std::string		nick;
	std::string		mail;
}					t_admin;

typedef struct		s_link
{
	std::string		servername;
	std::string		hostname;		// ip or domain
	std::string		pass;
	int				port;
	bool			tls;
	bool			ssl;
	bool			autoconnect;
}					t_link;

typedef struct		s_oper
{
	int				fd;
	std::string		name;
	std::string		pass;
	std::string		swhois;
	std::string		hostmask;
}					t_oper;

typedef struct		s_service
{
	int				fd;
	int				hopcount;
	std::string		name;
	std::string		pass;
	std::string		distribution;
	std::string		type;
	std::string		info;
	std::string		hostmask;
	std::string		token;
}					t_service;

typedef struct		s_whowas
{
	std::string		nickname;
	std::string		username;
	std::string		realname;
	std::string		hostname;
	std::string		servername;
	time_t			dtloggedin;
}					t_whowas;

typedef struct		s_kill
{
	std::string		nick;
	std::string		host;
	time_t			time;
	std::string		cause;
}					t_kill;

struct				blocked
{
	int					fd;
	time_t				_blocked_time;
	std::string			servername;
	std::list<Channel*>	channels;
	std::list<Client*>	clients;
};

struct				IRCserv
{
	typedef std::MAP<std::string, Command>	t_cmdmap;
	typedef std::MAP<int, t_fd>				t_fdsmap;
	std::string					servername;	// me server name
	std::string					token;		// me server token
	std::string					info;		// me server info
	t_fdsmap					fds;
	t_cmdmap					cmds;		// map of commands
	std::vector<t_listen>		listen;		// vector of sockets and ips to bind to listen
	t_admin						admin;		// for ADMIN command
	std::vector<t_link>			link;		// servers allowed to connect to
	std::vector<t_oper>			oper;		// operators thac can connect to server
	std::vector<t_server>		network;	// nearest servers connected to network
	std::list<Client>			clients;
	std::list<Channel>			channels;
	fd_set						fdset_read;
	fd_set						fdset_write;
	fd_set						fdset_error;
	std::string					motd_path;
	SSL_CTX						*sslctx;
	std::string					pass;		// this is argv server (ME) password
	int							port;		// this is argv server port
	std::string					usermodes;
	std::string					chanmodes;
	time_t						dtstarted;
	time_t						dtcompiled;
	std::list<t_whowas>			nickhistory;
	std::list<t_kill>			kills;
	std::list<blocked>			unavailable;
	std::list<t_service>		services;
};

void		parse(int ac, char **av, IRCserv *serv);
void		defineservermodes(IRCserv *serv);
void		clear_kill_list(IRCserv *serv);
void		clear_block_list(IRCserv *serv, std::string const &servername = "");
void		clear_empty_channels(IRCserv *serv);
void		clear_whowas(IRCserv *serv);
void		check_liveness(IRCserv *serv);
void		ProcessMessage(int fd, std::string const &msg, IRCserv *serv);

void		RunServer(IRCserv *serv);

//	basic connection
void		CreateSock(IRCserv *serv, t_listen &listen);
void		AcceptConnect(int _socket, IRCserv *serv, bool isTLS);
void		ReceiveMessage(int fd, IRCserv *serv);
void		SendMessage(int fd, IRCserv *serv);
bool		didSockFail(int fd, IRCserv *serv);

//	tls connection
void		InitSSLCTX(IRCserv *serv);
void		CreateSockTLS(IRCserv *serv, t_listen &listen);
void		DoHandshakeTLS(int fd, IRCserv *serv);

#endif
