/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 13:14:43 by salec             #+#    #+#             */
/*   Updated: 2020/11/27 23:35:59 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_HPP
# define IRCSERV_HPP

# include <iostream>
# include <exception>
# include <string>
# include <vector>
# include <list>
# include <map>
# include <algorithm>
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
# include "common_defines.hpp"
# include "reply_codes.hpp"
# include "error_codes.hpp"
# include "tools.hpp"
# include "client.hpp"
// #include <unordered_map>

typedef struct		s_fd
{
	int				type;
	bool			tls;
	bool			status;     // for disconnect assign false
	std::string		rdbuf;
	std::string		wrbuf;
	std::string		pass;		// this struct parameters required for
	std::string		version;	// message PASS that can be received either
	std::string		flags;		// from client or server before SERVER or NICK
	std::string		options;	// register attempt
	std::string		hostname;
	SSL				*sslptr;
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
	std::list<Client>			clients;
}					t_server;

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
}					t_oper;

struct				IRCserv
{
	typedef void (*t_command)(int fd, const t_strvect &split, IRCserv *serv);
	typedef std::map<std::string, t_command>	t_cmdmap;
	std::string					servername;	// me server name
	std::string					version;
	std::string					token;		// me server token
	std::string					info;		// me server info
	std::map<int, t_fd>			fds;
	t_cmdmap					command;	// map of commands
	std::vector<t_listen>		listen;		// vector of sockets and ips to bind to listen
	t_admin						admin;		// for ADMIN command
	std::vector<t_link>			link;		// servers allowed to connect to
	std::vector<t_oper>			oper;		// operators thac can connect to server
	std::vector<t_server>		network;	// servers that connected to network
	std::vector<Client>			clients;
	fd_set						fdset_read;
	fd_set						fdset_write;
	fd_set						fdset_error;
	std::string					motd_path;
	SSL_CTX						*sslctx;
	std::string					pass;		// this is argv server (ME) password
	int							port;		// this is argv server port
	std::string					dtcreated;
	std::string					usermodes;
	std::string					chanmodes;
};

void		parse(int ac, char **av, IRCserv *serv);
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
