/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_info.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/28 17:55:08 by salec             #+#    #+#             */
/*   Updated: 2020/11/28 18:03:02 by salec            ###   ########.fr       */
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
	INFO csd.bu.edu		; request an INFO reply from
						csd.bu.edu
	:Avalon INFO *.fi	; INFO request from Avalon for first
						server found to match *.fi.
	INFO Angel			; request info from the server that
						Angel is connected to.
*/

void	cmd_info(int fd, const t_strvect &split, IRCserv *serv)
{
	t_citer		cit = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	std::string	target = "";
	if (cit != serv->clients.end())
		target = cit->getnickname();

	if (split.size() > 1)
		std::cerr << "info does not support other servers yet" << std::endl;
	std::cerr << "info is not working yet" << std::endl;
/*	std::string	verdbg = serv->version + ".";
	std::string	comment = "release build";
	if (DEBUG_MODE)
	{
		verdbg += "debug build";
		comment = "release build";
	}
	verdbg += " " + serv->servername;
	serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
		RPL_VERSION, target, verdbg, comment);
*/
}
