/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_motd.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/21 17:32:05 by salec             #+#    #+#             */
/*   Updated: 2020/11/21 17:48:03 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

void	cmd_motd(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string	nick;
	t_citer		it;

	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
	if (it != serv->clients.end())
		nick = it->getnickname();
	if (split.size() < 2)
		serv->fds[fd].wrbuf += reply_motd(serv, nick);
	else
	{
		// we need to get other server motd if arg supplied
		serv->fds[fd].wrbuf += reply_motd(serv, nick);	// placeholder
	}
}
