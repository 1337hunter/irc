/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_notice.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/09 16:32:14 by salec             #+#    #+#             */
/*   Updated: 2021/01/17 17:50:38 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

/*
	Command: NOTICE
	Parameters: <msgtarget> <text>

	The NOTICE command is used similarly to PRIVMSG.  The difference
	between NOTICE and PRIVMSG is that automatic replies MUST NEVER be
	sent in response to a NOTICE message.  This rule applies to servers
	too - they MUST NOT send any error reply back to the client on
	receipt of a notice.  The object of this rule is to avoid loops
	between clients automatically sending something in response to
	something it received.

	This command is available to services as well as users.

	This is typically used by services, and automatons (clients with
	either an AI or other interactive program controlling their actions).

	See PRIVMSG for more details on replies and examples.
*/

void	cmd_notice(int fd, const t_strvect &split, IRCserv *serv)
{
	t_fd		&fdref = serv->fds[fd];
	t_service	*sptr = NULL;
	std::string	from = "";
	if (fdref.type == FD_SERVER && split[0].size() > 1 && split[0][0] == ':')
		from = split[0].substr(1);

	if (fdref.type == FD_SERVICE)
	{
		Client		*client_msg;
		t_service	*service = find_service_by_fd(fd, serv);
		if (split[1].find_first_of("%!@") == NPOS)
			client_msg = find_client_by_nick(split[1], serv);
		else
			client_msg = find_client_by_user_or_nick_and_host(split[1], serv);

		serv->fds[client_msg->getFD()].wrbuf += ":" + service->name +
			" NOTICE " + client_msg->getnick() + " " +
			strvect_to_string(split, ' ', 2) + CRLF;
		return ;
	}
	else if (fdref.type == FD_SERVER &&
		(sptr = find_service_by_name(from, serv)))
	{
		// send notice from service from another server
	}
	else
		cmd_privmsg(fd, split, serv);	// privmsg never replies to NOTICE
}
