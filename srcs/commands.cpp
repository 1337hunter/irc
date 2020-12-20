/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/20 18:40:06 by salec             #+#    #+#             */
/*   Updated: 2020/12/20 20:12:51 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

void	initcommands(IRCserv *serv)
{
	serv->cmds["USER"] 		= cmd_user;
	serv->cmds["NICK"] 		= cmd_nick;
	serv->cmds["PING"] 		= cmd_ping;
	serv->cmds["QUIT"] 		= cmd_quit;
	serv->cmds["PASS"] 		= cmd_pass;
	serv->cmds["SERVER"] 	= cmd_server;
	serv->cmds["SQUIT"] 	= cmd_squit;
	serv->cmds["CONNECT"] 	= cmd_connect;
	serv->cmds["OPER"] 		= cmd_oper;
	serv->cmds["ERROR"] 	= cmd_error;
	serv->cmds["ADMIN"] 	= cmd_admin;
	serv->cmds["MOTD"] 		= cmd_motd;
	serv->cmds["USERHOST"] 	= cmd_userhost;
	serv->cmds["VERSION"] 	= cmd_version;
	serv->cmds["INFO"] 		= cmd_info;
	serv->cmds["TIME"] 		= cmd_time;
	serv->cmds["JOIN"] 		= cmd_join;
	serv->cmds["PRIVMSG"] 	= cmd_privmsg;
	serv->cmds["INVITE"] 	= cmd_invite;
	serv->cmds["NAMES"] 	= cmd_names;
	serv->cmds["WHO"] 		= cmd_who;
	serv->cmds["WHOIS"] 	= cmd_whois;
	serv->cmds["WHOWAS"] 	= cmd_whowas;
	serv->cmds["PART"] 		= cmd_part;
	serv->cmds["MODE"] 		= cmd_mode;
	serv->cmds["NOTICE"] 	= cmd_notice;
	serv->cmds["AWAY"] 		= cmd_away;
	serv->cmds["KILL"] 		= cmd_kill;
	serv->cmds["LINKS"] 	= cmd_links;
	serv->cmds["STATS"] 	= cmd_stats;
	serv->cmds["NJOIN"] 	= cmd_njoin;
	serv->cmds["LUSERS"] 	= cmd_lusers;

	// values below are from InspIRCd-3 (need to check what it means)
	serv->usermodes = "iosw";
	serv->chanmodes = "biklmnopstv";
/*	// values below are from ircd v2.11.2p3
	serv->usermodes = "aoOirw";
	serv->chanmodes = "abeiIklmnoOpqrRstv"; */
}

Command::Command() : cmd(NULL), count(0), bytes(0), rcount(0) {}
Command::~Command() {}
Command::Command(t_command cmd) : cmd(cmd), count(0), bytes(0), rcount(0) {}
Command::Command(Command const &other) : cmd(other.cmd), count(other.count),
	bytes(other.bytes), rcount(other.rcount) {}
Command		&Command::operator=(Command const &other)
{
	this->cmd = other.cmd;
	this->count = other.count;
	this->bytes = other.bytes;
	this->rcount = other.rcount;
	return (*this);
}
uint	Command::getcount(void) { return (this->count); }
size_t	Command::getbytes(void) { return (this->bytes); }
uint	Command::getrcount(void) { return (this->rcount); }
bool	Command::used(void)
{
	if (cmd != NULL && (count > 0 || rcount > 0 || bytes > 0))
		return (true);
	return (false);
}

void	Command::Execute(int fd, const t_strvect &split, IRCserv *serv,
	size_t bytes, bool remote)
{
	if (this->cmd == NULL)
		return ;
//	increase usage stats
	this->bytes += bytes;
	this->count++;
	if (remote)
		this->rcount++;
//	execute
	this->cmd(fd, split, serv);
}
