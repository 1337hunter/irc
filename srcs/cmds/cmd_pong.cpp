/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_pong.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/17 19:46:48 by salec             #+#    #+#             */
/*   Updated: 2021/01/17 19:51:49 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

void		cmd_pong(int fd, const t_strvect &split, IRCserv *serv)
{
	if (split.size() < 2)
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NOORIGIN, "", "", "No origin specified");
	else
	{
		serv->fds[fd].status = true;
	}
}
