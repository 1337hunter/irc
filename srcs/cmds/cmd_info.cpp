/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_info.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/28 17:55:08 by salec             #+#    #+#             */
/*   Updated: 2020/12/19 03:08:31 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*
	Command: INFO
	Parameters: [<server>]

	The INFO command is required to return information which describes
	the server: its version, when it was compiled, the patchlevel, when
	it was started, and any other miscellaneous information which may be
	considered to be relevant.

	Numeric Replies:
		ERR_NOSUCHSERVER
		RPL_INFO
		RPL_ENDOFINFO

	Examples:
	INFO csd.bu.edu		; request an INFO reply from csd.bu.edu
	:Avalon INFO *.fi	; INFO request from Avalon for first server found
						to match *.fi.
	INFO Angel			; request info from the server that Angel
						is connected to.
*/

std::string	reply_info(IRCserv *serv, std::string const &target)
{
	std::string	reply = "";
	t_strvect	infovect;
	std::string	buildinfo = "release build";
	if (DEBUG_MODE)
		buildinfo = "debug build";

	infovect.push_back("\t\t\t" + serv->versionstring + " " + buildinfo);
	infovect.push_back("Developed since October 2020 till present");
	infovect.push_back("");
	infovect.push_back("Compiled:\t\t" + ft_timetostring(serv->dtcompiled));
	infovect.push_back("Started:\t\t\t" + ft_timetostring(serv->dtstarted));
	infovect.push_back("");
	infovect.push_back("Created by:");
	infovect.push_back("\t\tgbright");
	infovect.push_back("\t\tsalec");
	infovect.push_back("");
	infovect.push_back("This is an early ircserv build for ft_irc project");

	for (size_t i = 0; i < infovect.size(); i++)
		reply += ft_buildmsg(serv->servername,
			RPL_INFO, target, "", infovect[i]);
	reply += ft_buildmsg(serv->servername,
		RPL_ENDOFINFO, target, "", "End of INFO list");
	return (reply);
}

void		cmd_info(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string	nick = getnicktoreply(fd, split, serv);
	if (nick.empty())
	{
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");
		return ;
	}

	if (serv->fds[fd].type != FD_SERVER && (split.size() < 2 ||
		(split.size() > 1 && getservernamebymask(serv, split[1]) == serv->servername)))
		serv->fds[fd].wrbuf += reply_info(serv, nick);
	else if (serv->fds[fd].type != FD_SERVER && split.size() > 1)
	{
		int	servfd = getserverfdbymask(serv, split[1]);
		if (servfd > 0)
			serv->fds[servfd].wrbuf += ":" + nick + " INFO " + split[1] + CRLF;
		else
			serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
				ERR_NOSUCHSERVER, nick, split[1], "No such server");
	}
	else if (split.size() >= 3)	// from another server: reply or forward
	{
		nick = split[0].substr(1);
		if (getservernamebymask(serv, split[2]) == serv->servername)
			serv->fds[fd].wrbuf += reply_info(serv, nick);
		else
		{
			int	servfd = getserverfdbymask(serv, split[2]);
			if (servfd > 0)
				serv->fds[servfd].wrbuf += split[0] + " INFO " + split[2] + CRLF;
			else
				serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
					ERR_NOSUCHSERVER, nick, split[1], "No such server");
		}
	}
}
