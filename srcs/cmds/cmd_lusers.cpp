/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_lusers.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/19 15:04:36 by salec             #+#    #+#             */
/*   Updated: 2020/12/19 17:33:11 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*
	Command: LUSERS
	Parameters: [ <mask> [ <target> ] ]

	The LUSERS command is used to get statistics about the size of the
	IRC network.  If no parameter is given, the reply will be about the
	whole net.  If a <mask> is specified, then the reply will only
	concern the part of the network formed by the servers matching the
	mask.  Finally, if the <target> parameter is specified, the request
	is forwarded to that server which will generate the reply.
	Wildcards are allowed in the <target> parameter.

	Numeric Replies:
		RPL_LUSERCLIENT
		RPL_LUSEROP
		RPL_LUSERUNKNOWN
		RPL_LUSERCHANNELS
		RPL_LUSERME
		ERR_NOSUCHSERVER
*/

void		cmd_lusers(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string	nick;
	t_citer		it;

	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
		nick = it->getnickname();
	else if (split[0][0] == ':')
		nick = split[0].substr(1);
	else
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");

	if (serv->fds[fd].type != FD_SERVER && (split.size() < 3 ||
		(split.size() > 2 && getservernamebymask(serv, split[2]) == serv->servername)))
	{
		if (split.size() < 2)
			serv->fds[fd].wrbuf += reply_lusers(serv, nick);
		else
			serv->fds[fd].wrbuf += reply_lusers(serv, nick, split[1]);
	}
	else if (serv->fds[fd].type != FD_SERVER && split.size() > 2)
	{
		int	servfd = getserverfdbymask(serv, split[2]);
		if (servfd > 0)
			serv->fds[servfd].wrbuf += ":" + nick + " LUSERS " +
				split[1] + " " + split[2] + CRLF;
		else
			serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
				ERR_NOSUCHSERVER, nick, split[2], "No such server");
	}
	else if (split.size() >= 4)	// from another server: reply or forward
	{
		if (getservernamebymask(serv, split[3]) == serv->servername)
			serv->fds[fd].wrbuf += reply_lusers(serv, nick, split[2]);
		else
		{
			int	servfd = getserverfdbymask(serv, split[3]);
			if (servfd > 0)
				serv->fds[servfd].wrbuf += split[0] + " LUSERS " +
					split[2] + " " + split[3] + CRLF;
			else
				serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, split[3], "No such server");
		}
	}
}
