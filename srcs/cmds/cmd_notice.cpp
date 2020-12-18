/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_notice.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/09 16:32:14 by salec             #+#    #+#             */
/*   Updated: 2020/12/18 18:29:54 by salec            ###   ########.fr       */
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
	(void)fd;	// check if fd is other server
	(void)serv;
	if (split.size() != 3)
		return ;

	// so basically it just sends somebody notice without any replies
	// need to find where to send it
	// if there's noone to send just silently forget it
}
