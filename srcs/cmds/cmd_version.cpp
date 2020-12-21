/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_version.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/28 17:17:42 by salec             #+#    #+#             */
/*   Updated: 2020/12/21 18:32:26 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*
	Command: VERSION
	Parameters: [ <target> ]

	The VERSION command is used to query the version of the server
	program.  An optional parameter <target> is used to query the version
	of the server program which a client is not directly connected to.
	Wildcards are allowed in the <target> parameter.

	Numeric Replies:
		ERR_NOSUCHSERVER
		RPL_VERSION

	Examples:
	VERSION tolsun.oulu.fi		; Command to check the version of
								server "tolsun.oulu.fi".
*/

void	cmd_version(int fd, const t_strvect &split, IRCserv *serv)
{
	t_fd		&fdref = serv->fds[fd];
	std::string	nick = getnicktoreply(fd, split, serv);
	if (nick.empty())
	{
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");
		return ;
	}

	std::string	verdbg(VERSIONSTRING);
	verdbg += ".";
	std::string	comment = "release build";
	if (DEBUG_MODE)
	{
		verdbg += "debug";
		comment = "debug build";
	}
	verdbg += " " + serv->servername;

	if (fdref.type != FD_SERVER && (split.size() < 2 ||
		(split.size() > 1 && getservernamebymask(serv, split[1]) == serv->servername)))
	{
		fdref.wrbuf += ft_buildmsg(serv->servername,
			RPL_VERSION, nick, verdbg, comment);
	}
	else if (fdref.type != FD_SERVER && split.size() > 1)
	{
		int	servfd = getserverfdbymask(serv, split[1]);
		if (servfd > 0)
			serv->fds[servfd].wrbuf += ":" + nick + " VERSION " + split[1] + CRLF;
		else
			fdref.wrbuf += ft_buildmsg(serv->servername,
				ERR_NOSUCHSERVER, nick, split[1], "No such server");
	}
	else if (split.size() >= 3)	// from another server: reply or forward
	{
		nick = split[0].substr(1);
		if (getservernamebymask(serv, split[2]) == serv->servername)
		{
			fdref.wrbuf += ft_buildmsg(serv->servername,
				RPL_VERSION, nick, verdbg, comment);
		}
		else
		{
			int	servfd = getserverfdbymask(serv, split[2]);
			if (servfd > 0)
				serv->fds[servfd].wrbuf += split[0] + " VERSION " + split[2] + CRLF;
			else
				fdref.wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, split[1], "No such server");
		}
	}
}
