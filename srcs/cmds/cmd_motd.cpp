/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_motd.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/21 17:32:05 by salec             #+#    #+#             */
/*   Updated: 2020/12/16 16:54:11 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

/*
	Command: MOTD
	Parameters: [ <target> ]

	The MOTD command is used to get the "Message Of The Day" of the given
	server, or current server if <target> is omitted.
	Wildcards are allowed in the <target> parameter.

	Numeric Replies:
		RPL_MOTDSTART
		RPL_MOTD
		RPL_ENDOFMOTD
		ERR_NOMOTD
*/

void	cmd_motd(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string	nick;
	t_citer		it;

	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
		nick = it->getnickname();

	if (serv->fds[fd].type != FD_SERVER && (split.size() < 2 ||
		(split.size() > 1 && getservernamebymask(serv, split[1]) == serv->servername)))
	{
		serv->fds[fd].wrbuf += reply_motd(serv, nick);
	}
	else if (serv->fds[fd].type != FD_SERVER && split.size() > 1)
	{
		int	servfd = getserverfdbymask(serv, split[1]);
		if (servfd > 0 && !nick.empty())
			serv->fds[servfd].wrbuf += ":" + nick + " MOTD " + split[1] + CRLF;
	}
	else if (split.size() >= 3)	// from another server: reply or forward
	{
		nick = split[0].substr(1);
		if (getservernamebymask(serv, split[2]) == serv->servername)
			serv->fds[fd].wrbuf += reply_motd(serv, nick);
		else
		{
			int	servfd = getserverfdbymask(serv, split[2]);
			if (servfd > 0)
				serv->fds[servfd].wrbuf += split[0] + " MOTD " + split[2] + CRLF;
		}
	}
}
