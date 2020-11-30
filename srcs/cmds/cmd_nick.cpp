/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_nick.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:29:56 by salec             #+#    #+#             */
/*   Updated: 2020/11/30 17:06:42 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

//Command: NICK
//Parameters: <nickname>[1] <hopcount>[2] <username>[3] <host>[4] <servertoken>[]
// <umode>[6] <realname>[7]
void	add_nick_to_routing(int fd, const t_strvect &split, IRCserv *serv)
{
	t_server				*routing;
	int						hop;
	Client					newone(split);
	std::string				forward;
	std::vector<t_server>::iterator	net;

	try { hop = stoi(split[2]); } catch (std::exception &e) { (void)e; return ; }
	if ((routing = find_server_by_fd(fd, serv)) != 0)
		routing->clients.push_back(newone); //else error
	forward = "NICK " + split[1] + " " + std::to_string(hop + 1) +
		" " + split[3] + " " + split[4] + " " + split[5] + " " + split[6] +
		" " + split[7];
	for (size_t i = 8; i < split.size(); i++)
		forward += " " + split[i];
	forward += CRLF;

	net = serv->network.begin();
	for (; net != serv->network.end(); net++)
		if (net->fd != fd)
			serv->fds[net->fd].wrbuf += forward;
}

void	cmd_nick(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string		reply;
	Client			*client;
	t_citer			nick_entry;
	t_citer			fd_entry;

	if (split.size() < 2)
	{
		// may be different
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NONICKNAMEGIVEN, "", "NICK", "No nickname given");
		return ;
	}
	if (split.size() > 7 && serv->fds[fd].type == FD_SERVER)
	{
		add_nick_to_routing(fd, split, serv);
		return ;
	}
	client = find_client_by_nick(split[1], serv);
	fd_entry = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	nick_entry = ft_findnick(serv->clients.begin(), serv->clients.end(), split[1]);
	if (nick_entry == serv->clients.end() && fd_entry == serv->clients.end() && client == 0)
	{
		serv->clients.push_back(Client(split[1], fd));
		serv->clients.back().sethostname(serv->fds[fd].hostname);
	}
	else if (nick_entry != serv->clients.end() && nick_entry->isConnected() &&
			!nick_entry->isRegistred() && fd_entry == serv->clients.end() &&
			client->gethopcount() == "0")
	{
		std::cout << "2\n";
		serv->fds[nick_entry->getFD()].status = false;
		serv->fds[nick_entry->getFD()].wrbuf = "Error :Closing Link: " + nick_entry->getnickname() + " (Overridden)";
		serv->fds[nick_entry->getFD()].wrbuf += CRLF;
		nick_entry->setFD(fd);
		nick_entry->sethostname(serv->fds[fd].hostname);
	}
	else if (nick_entry != serv->clients.end() && !(nick_entry->isConnected()))
	{
		std::cout << "3\n";
		nick_entry->Reconnect(fd);
		nick_entry->sethostname(serv->fds[fd].hostname);
	}
	else if (fd_entry != serv->clients.end() && fd_entry->isRegistred())
	{
		std::cout << "4\n";
		// Here we also need to track nicknames
		// for KICK, MODE and KILL commands.
		reply = ":";
		reply += fd_entry->getnickname();
		fd_entry->ChangeNick(split[1]);
		reply += " NICK ";
		reply += fd_entry->getnickname();
		reply += CRLF;
	}
	else if (fd_entry != serv->clients.end() && !fd_entry->isRegistred())
	{
		std::cout << "5\n";
		fd_entry->sethostname(serv->fds[fd].hostname);
		fd_entry->Register(split[1]);
		if (fd_entry->getUSER())
		{
			nick_forward(serv, fd_entry);
			reply = reply_welcome(serv, fd_entry);
		}
	}
	else
	{
		std::cout << "6\n";
		reply = ft_buildmsg(serv->servername, ERR_NICKNAMEINUSE, split[1], "",
			"Nickname is already in use");
		/*	need to save the state in this case
		 *	cli will try to send another NICK after USER msg */
	}
	serv->fds[fd].wrbuf += reply;
}
