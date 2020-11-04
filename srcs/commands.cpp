/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/27 15:24:34 by gbright           #+#    #+#             */
/*   Updated: 2020/11/04 12:11:08 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

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
		reply = ":" + _server->hostname + " ";
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
		reply = ":" + _server->hostname + " ";
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
				reply = ":" + _server->hostname + " ";
				reply += RPL_WELCOME;
				reply += " " + it->getnickname() +
					" :Welcome to the Internet Relay Network " +
					it->getnickname() + "!" +
					it->getnickname() + "@" + _server->hostname + CLRF;
			}
			else
			{
				reply = ":" + _server->hostname + " ";
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

	if (split.size() < 2)
	{
		reply = ":" + _server->hostname + " ";
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
	std::cout << "client " << fd << ":\t\t\tdisconnected" << std::endl;
#endif
}

void		cmd_server(int fd, const t_strvect &split, IRCserv *_server)
{
	server_server	temp;
#if DEBUG_MODE
	std::cout << "incoming connection from:\t" << split[1] << std::endl;
#endif
	if (split.size() < 4) // is info parameter counts?
	{
		std::string	reply;
		reply = "ERROR :Not enough SERVER parameters";
		reply += CLRF;
		send(fd, reply.c_str(), reply.length(), 0);
	}
	std::vector<server_server>::iterator	begin = _server->connect.begin();
	std::vector<server_server>::iterator	end = _server->connect.end();
	while (begin != end) //looking for servers with the same name
	{
		if (begin->host == split[1])
		{
			std::string	reply;
			reply = ":" + _server->hostname + " ";
			reply += ERR_ALREADYREGISTRED;
			reply += " " + split[1];
			reply += " :server already registred";
			reply += CLRF;
			send(fd, reply.c_str(), reply.length(), 0);
			cmd_squit(fd, split, _server);
			break ;

		}
		begin++;
	}
	if (!(_server->fds[fd].pass == _server->pass || _server->pass == ""))
	{
#if DEBUG_MODE
		std::cout << "client " << fd << "\t\twrong server password" << std::endl;
#endif
		cmd_squit(fd, split, _server);
	}
	temp.host = split[1];
	try { temp.hopcount = stoi(split[2]); temp.token = stoi(split[3]); }
	catch (std::exception &e)
	{
#if DEBUG_MODE
		std::cerr << "Error: bad cast hopcount or token. Connection is terminated.";
		std::cerr << std::endl;
		cmd_squit(fd, split, _server);
#endif
		return ;
	}
	temp.info = split[4];
	std::string	broadcast;
	std::vector<std::string>::const_iterator	itbegin = split.begin();
	std::vector<std::string>::const_iterator	itend = split.end();
	size_t	i = 0;
	while (itbegin != itend)
	{
		if (i != 2)
			broadcast += *itbegin;
		else
			broadcast += std::to_string(temp.hopcount + 1);
		if (itbegin + 1 != itend)
			broadcast += " ";
		itbegin++;
		i++;
	}
	std::map<int, t_fd>::iterator	mbegin = _server->fds.begin();
	std::map<int, t_fd>::iterator	mend = _server->fds.end();
	while (mbegin != mend)
	{
		if (mbegin->second.type == FD_SERVER && mbegin->first != _server->sock)
			send(mbegin->first, broadcast.c_str(), broadcast.length(), 0);
		mbegin++;
	}
	_server->fds[fd].type = FD_SERVER;
	_server->connect.push_back(temp);
}

void            cmd_pass(int fd, const t_strvect &split, IRCserv *_server)
{
	if (split.size() < 2)
	{
		std::string	reply;
		reply = ":" + _server->hostname + " ";
		reply += ERR_NEEDMOREPARAMS;
		reply += " :not enough parameters";
		reply += CLRF;
		send(fd, reply.c_str(), reply.length(), 0);
		return ;
	}
	_server->fds[fd].pass = split[1];
	if (split.size() == 2)
		return ;
	_server->fds[fd].version = split[2];
	if (split.size() == 3)
		return ;
	_server->fds[fd].flags = split[3];
	if (split.size() == 4)
		return ;
	_server->fds[fd].options = split[4];
}

void            cmd_squit(int fd, const t_strvect &split, IRCserv *_server)
{
	std::vector<server_server>::iterator	begin;
	std::vector<server_server>::iterator	end;

	while (begin != end && begin->host != split[1])
		begin++;
	if (begin != end)
	{
		_server->connect.erase(begin);
		FD_CLR(fd, &(_server->fdset_read));
		close(fd);
		_server->fds.erase(fd);
#if DEBUG_MODE
		std::cout << "server " << split[1] << ":\t\t\tdisconnected" << std::endl;
#endif
	}
}

void		cmd_connect(int fd, const t_strvect &split, IRCserv *_server)
{
	fd = 0;
	(void)split;
	_server = 0;
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
