/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_users.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/17 20:31:42 by salec             #+#    #+#             */
/*   Updated: 2020/12/21 16:08:43 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*
	Command: USERS
	Parameters: [ <target> ]

	The USERS command returns a list of users logged into the server in a
	format similar to the UNIX commands who(1), rusers(1) and finger(1).
	If disabled, the correct numeric MUST be returned to indicate this.

	Because of the security implications of such a command, it SHOULD be
	disabled by default in server implementations.  Enabling it SHOULD
	require recompiling the server or some equivalent change rather than
	simply toggling an option and restarting the server.  The procedure
	to enable this command SHOULD also include suitable large comments.

	Numeric Replies:
		ERR_NOSUCHSERVER
		ERR_FILEERROR
		RPL_USERSSTART
		RPL_USERS
		RPL_NOUSERS
		RPL_ENDOFUSERS
		ERR_USERSDISABLED
	Disabled Reply:
		ERR_USERSDISABLED

	Example:
	USERS eff.org	; request a list of users logged in on server eff.org
*/

std::string	reply_users(IRCserv *serv, std::string const &target)
{
	std::string	reply = "";

	reply += ft_buildmsg(serv->servername, ERR_USERSDISABLED, target, "",
		"USERS has been disabled");
	return (reply);
}

void		cmd_users(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string	nick;
	t_citer		it;
	t_fd		&fdref = serv->fds[fd];

	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
		nick = it->getnick();
	else if (split[0][0] == ':')
		nick = split[0].substr(1);
	else
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOTREGISTERED, "", "", "You have not registered");

	if (fdref.type != FD_SERVER && (split.size() < 3 ||
		(split.size() > 2 && getservernamebymask(serv, split[1]) == serv->servername)))
	{
		fdref.wrbuf += reply_users(serv, nick);
	}
	else if (fdref.type != FD_SERVER && split.size() > 2)
	{
		fdref.wrbuf += reply_users(serv, nick);
	}
	else if (split.size() >= 4)	// from another server: reply or forward
	{
		fdref.wrbuf += reply_users(serv, nick);
	}
}
