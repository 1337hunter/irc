/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_ping.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:35:05 by salec             #+#    #+#             */
/*   Updated: 2020/11/06 19:54:39 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_ping(int fd, const t_strvect &split, IRCserv *_server)
{
	if (split.size() < 2)
		_server->fds[fd].wrbuf += ft_buildmsg(_server->hostname,
			ERR_NEEDMOREPARAMS, "", "", "No origin specified");
	else
		_server->fds[fd].wrbuf += "PONG " + split[1] + CLRF;
}
