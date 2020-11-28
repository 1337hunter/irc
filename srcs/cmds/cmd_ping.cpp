/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_ping.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:35:05 by salec             #+#    #+#             */
/*   Updated: 2020/11/28 12:52:33 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

void		cmd_ping(int fd, const t_strvect &split, IRCserv *serv)
{
	if (split.size() < 2)
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NEEDMOREPARAMS, "", "", "No origin specified");
	else
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername, "PONG",
			serv->servername, "", split[1]);
	/*	:<hostname> PONG <hostname> :<second ping parameter>	*/
}
