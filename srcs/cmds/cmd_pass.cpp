/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_pass.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:35:58 by salec             #+#    #+#             */
/*   Updated: 2020/11/28 12:51:50 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

void		cmd_pass(int fd, const t_strvect &split, IRCserv *serv)
{
	if (split.size() < 2)
	{
		serv->fds[fd].wrbuf += ft_buildmsg(serv->servername,
			ERR_NEEDMOREPARAMS, "", split[0], "Not enough parameters");
		return ;
	}
	serv->fds[fd].pass = split[1];
	if (split.size() == 2)
		return ;
	serv->fds[fd].version = split[2];
	if (split.size() == 3)
		return ;
	serv->fds[fd].flags = split[3];
	if (split.size() == 4)
		return ;
	serv->fds[fd].options = split[4];
}
