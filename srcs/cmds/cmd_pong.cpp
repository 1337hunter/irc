/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_pong.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/17 19:46:48 by salec             #+#    #+#             */
/*   Updated: 2021/01/17 21:00:17 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

void		cmd_pong(int fd, const t_strvect &split, IRCserv *serv)
{
	t_fd	&fdref = serv->fds[fd];

	if (split.size() < 2)
		fdref.wrbuf += ft_buildmsg(serv->servername,
			ERR_NOORIGIN, "", "", "No origin specified");
	if (split.size() == 2 && split[1] == serv->servername)
	{
		fdref.awaitingpong = false;
		fdref.lastactive = ft_getcurrenttime();
	}
	if (split.size() >= 3)
	{
		// then forward to split[2]
	}
}
