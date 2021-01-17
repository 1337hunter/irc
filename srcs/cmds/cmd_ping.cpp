/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_ping.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:35:05 by salec             #+#    #+#             */
/*   Updated: 2021/01/18 00:27:35 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

void		cmd_ping(int fd, const t_strvect &split, IRCserv *serv)
{
	t_fd	&fdref = serv->fds[fd];

	if (split.size() < 2)
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOORIGIN, "", "", "No origin specified");
	else if (split.size() == 2)
		fdref.wrbuf += ft_buildmsg(serv->servername, "PONG",
			serv->servername, "", split[1]);
	else if (split.size() >= 3)
	{
		// then forward to split[2]
	}
}
