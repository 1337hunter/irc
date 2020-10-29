/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/27 15:24:34 by gbright           #+#    #+#             */
/*   Updated: 2020/10/29 22:57:36 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

t_citer		ft_findclientfd(t_citer const &begin, t_citer const &end, int fd)
{
	for (t_citer it = begin; it != end; it++)
		if (it->isConnected() && it->getFD() == fd)
			return (it);
	return (end);
}

t_citer		ft_findnick(t_citer const &begin, t_citer const &end, std::string const &nick)
{
	for (t_citer it = begin; it != end; it++)
		if (it->getnickname() == nick)
			return (it);
	return (end);
}

void		cmd_nick(int fd, const t_strvect &split, IRCserv *_server)
{
	std::string		reply;
	t_citer			nick_entry;
	t_citer			fd_entry;
	t_citer			end;

	end = _server->clients.end();
	fd_entry = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
	nick_entry = ft_findnick(_server->clients.begin(), _server->clients.end(), split[1]);
	if (nick_entry == end && fd_entry == end)
		_server->clients.push_back(Client(split[1], fd));
	else if (fd_entry != end)
	{
		// Here we also need to track nicknames
		// for KICK, MODE and KILL commands.
		reply = ":";
		reply += fd_entry->getnickname();
		fd_entry->ChangeNick(split[1]);
		reply += " NICK ";
		reply += fd_entry->getnickname();
		reply += CLRF;
		send(fd, reply.c_str(), reply.length(), 0);
	}
	else
	{
		reply = ":localhost ";
		reply += ERR_NICKNAMEINUSE;
		reply += " " + split[1] + " :Nickname is already in use";
		reply += CLRF;
		send(fd, reply.c_str(), reply.length(), 0);
		/*	need to save the state in this case
		 *	cli will try to send another NICK after USER msg */
	}
}

void		cmd_user(int fd, const t_strvect &split, IRCserv *_server)
{
	std::string		reply;
	t_citer			it;

	if (split.size() < 5)
	{
		reply = ":localhost ";
		reply += ERR_NEEDMOREPARAMS;
		reply += " " + split[0];
		reply += " :Not enough parameters";
		reply += CLRF;
	}
	else
	{
		it = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
		if (it != _server->clients.end())
		{
			if (it->Register(split[1], split[4]))
			{
				reply = ":localhost ";
				reply += RPL_WELCOME;
				reply += " " + it->getnickname() +
					" :Welcome to the Internet Relay Network " +
					it->getnickname() + "!" +
					it->getnickname() + "@" + "localhost" + CLRF;
			}
			else
			{
				reply = ":localhost ";
				reply += ERR_ALREADYREGISTRED;
				reply += " :Unauthorized command (already registered)";
				reply += CLRF;
			}
		}
	}
	send(fd, reply.c_str(), reply.length(), 0);
}

void		cmd_ping(int fd, const t_strvect &split, IRCserv *_server)
{
	std::string		reply;

	_server = 0;
	if (split.size() < 2)
	{
		reply = ":localhost ";
		reply += ERR_NOORIGIN;
		reply += " :No origin specified";
		reply += CLRF;
	}
	else
		reply = "PONG " + split[1] + CLRF;
	send(fd, reply.c_str(), reply.length(), 0);
}

void		cmd_quit(int fd, const t_strvect &split, IRCserv *_server)
{
	(void)split;
	FD_CLR(fd, &(_server->fdset_read));
	close(fd);
	_server->fds.erase(fd);
	t_citer it = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
	if (it != _server->clients.end())
		it->Disconnect();
#if DEBUG_MODE
                std::cout << "Client " << fd << " disconnected" << std::endl;
#endif
}

void		cmd_server(int fd, const t_strvect &split, IRCserv *_server)
{
	fd = 0;
	(void)split;
#if DEBUG_MODE
	std::cout << "incoming connection from:\t" << split[1] << std::endl;
#endif
	_server->connect_from.push_back(split[1]);
}

/*
	if (split[0] == "WHO")
	{
		352	RPL_WHOREPLY
			"<channel> <user> <host> <server> <nick>
			<H|G>[*][@|+] :<hopcount> <real name>"
	}
	if (split[0] == "USERHOST")
	{
		302	RPL_USERHOST
		":[<reply>{<space><reply>}]"
		- Reply format used by USERHOST to list replies to
			the query list.  The reply string is composed as
			follows:

			<reply> ::= <nick>['*'] '=' <'+'|'-'><hostname>

			The '*' indicates whether the client has registered
			as an Operator.  The '-' or '+' characters represent
			whether the client has set an AWAY message or not
			respectively.
	}
*/
