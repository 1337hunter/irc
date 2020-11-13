/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/06 13:14:43 by salec             #+#    #+#             */
/*   Updated: 2020/11/13 20:24:02 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCSERV_HPP
# define IRCSERV_HPP

# include <iostream>
# include <exception>
# include <string>
# include <vector>
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
	bool			status;
	std::string		rdbuf;
	std::string		wrbuf;
	std::string		pass;	// this structer parameters required for
	std::string		version;// message PASS that can be received either
	std::string		flags;	// from client or server before SERVER or NICK
	std::string		options;// register attemption
}					t_fd;

typedef struct		s_server
{
	int				fd;
	int				hopcount;
	int				port;
	bool			autoconnect;
	std::string		token;
	std::string		hostname;
	std::string		pass;
	std::string		info;
}					t_server;

typedef struct		s_listen
{
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
	std::string		ip;
	std::string		hostname;
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
	int							port;
	int							sock;
	std::string					hostname;	//me server name
	std::string					token;		//me server token
	std::string					info;		// me server info
	std::map<int, t_fd>			fds;
	t_cmdmap					command; //map of commands
	std::string					pass;    //this server password
	std::vector<t_listen>		listen;  //vector of sockets amd ips to bind to listen
	t_admin						admin;   //for ADMIN command
	std::vector<t_link>			link;    //servers allowed to connect to
	std::vector<t_oper>			oper;    //operators thac can connect to server
	std::vector<t_server>		connect; //servers that connected to this server
	std::vector<Client>			clients;
	fd_set						fdset_read;
	fd_set						fdset_write;
	fd_set						fdset_error;
	static std::string const	clrf;
};

void		parse(int ac, char **av, IRCserv *serv);
void		CreateSock(IRCserv *serv);
void		AcceptConnect(IRCserv *serv);
void		RecieveMessage(int fd, IRCserv *serv);
void		ProcessMessage(int fd, std::string const &msg, IRCserv *serv);
void		RunServer(IRCserv *serv);

#endif
