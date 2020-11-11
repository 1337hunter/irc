/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_user.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:30:28 by salec             #+#    #+#             */
/*   Updated: 2020/11/11 17:14:36 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_user(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string		reply;
	t_citer			it;
	std::string		_realname;

	if (split.size() < 5)
	{
		reply = ft_buildmsg(serv->hostname, ERR_NEEDMOREPARAMS, "*", split[0],
			"Not enough parameters");
	}
	else
	{
		it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
		if (it != serv->clients.end())
		{
			if (split[4][0] == ':')
			{
				std::string realname(split[4]);
				size_t	i = 4;

				while (++i < split.size())
				{
					realname += " ";
					realname += split[i];
				}
				_realname = realname;
			}
			else
				_realname = split[4];
			if (it->Register(split[1], _realname))
				reply = reply_welcome(serv, it);
			else
				reply = ft_buildmsg(serv->hostname, ERR_ALREADYREGISTRED,
					it->getnickname(), "", "You may not reregister");
		}
	}
	serv->fds[fd].wrbuf += reply;
}
