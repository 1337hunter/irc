/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_pass.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:35:58 by salec             #+#    #+#             */
/*   Updated: 2020/11/06 19:44:33 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_pass(int fd, const t_strvect &split, IRCserv *_server)
{
	if (split.size() < 2)
	{
		_server->fds[fd].wrbuf += ft_buildmsg(_server->hostname,
			ERR_NEEDMOREPARAMS, "", split[0], "Not enough parameters");
		return ;
	}
	_server->fds[fd].pass = split[1];
	if (split.size() == 2)
		return ;
	_server->fds[fd].version = split[2];
	if (split.size() == 3)
		return ;
	_server->fds[fd].flags = split[3];
	if (split.size() == 4)
		return ;
	_server->fds[fd].options = split[4];
}
