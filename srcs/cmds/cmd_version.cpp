/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_version.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/28 17:17:42 by salec             #+#    #+#             */
/*   Updated: 2020/12/05 16:50:42 by salec            ###   ########.fr       */
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
	t_citer		cit = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	std::string	target = "";
	if (cit != serv->clients.end())
		target = cit->getnickname();

	if (split.size() > 1)
		std::cerr << "version does not support other servers yet" << std::endl;

	std::string	verdbg = serv->version + ".";
	std::string	comment = "release build";
	if (DEBUG_MODE)
	{
		verdbg += "debug";
		comment = "debug build";
	}
	verdbg += " " + serv->servername;
	serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
		RPL_VERSION, target, verdbg, comment);
}
