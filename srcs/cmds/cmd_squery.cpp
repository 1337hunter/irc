/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_squery.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/12 15:49:51 by salec             #+#    #+#             */
/*   Updated: 2021/01/12 16:05:33 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	Command: SQUERY
	Parameters: <servicename> <text>

	The SQUERY command is used similarly to PRIVMSG.  The only difference
	is that the recipient MUST be a service.  This is the only way for a
	text message to be delivered to a service.
	See PRIVMSG for more details on replies and example.

	Examples:
	SQUERY irchelp :HELP privmsg			; Message to the service with
											nickname irchelp.
	SQUERY dict@irc.fr :fr2en blaireau		; Message to the service with name
											dict@irc.fr.
*/

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

void	cmd_squery(int fd, const t_strvect &split, IRCserv *serv)
{
	if (split.size() < 3)
		return ;
#if DEBUG_MODE
	std::cout << "client " << fd <<
		" sent service query to " << split[1] <<
		" service:" << std::endl << split[2] << std::endl;
#endif
	(void)fd;
	(void)serv;
}
