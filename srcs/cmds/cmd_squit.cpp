/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_squit.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:37:57 by salec             #+#    #+#             */
/*   Updated: 2020/11/20 22:13:48 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_squit(int fd, const t_strvect &split, IRCserv *serv)
{
	std::vector<t_server>::iterator	begin = serv->network.begin();
	std::vector<t_server>::iterator	end = serv->network.end();

	if (split.size() == 1)
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "SQUIT",
				"Not enough parameters");
		return ;
	}
	while (begin != end && begin->servername != split[1])
		begin++;
	if (begin != end && begin->fd != -1)
	{
		close(begin->fd);
		serv->fds.erase(begin->fd);
		FD_CLR(begin->fd, &(serv->fdset_read));
		FD_CLR(begin->fd, &(serv->fdset_write));
		serv->network.erase(begin);
#if DEBUG_MODE
		std::cout << "server " << split[1] << ":\tdisconnected" << std::endl;
#endif
	}
	else
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHSERVER, fd, "",
				"No such server");
}
