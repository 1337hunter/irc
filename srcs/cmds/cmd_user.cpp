/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_user.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:30:28 by salec             #+#    #+#             */
/*   Updated: 2021/01/13 12:18:01 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"

void		cmd_user(int fd, const t_strvect &split, IRCserv *serv)
{
	std::string		reply;
	t_citer			it;
	std::string		_realname;

	if (split.size() < 5)
	{
		reply = ft_buildmsg(serv->servername, ERR_NEEDMOREPARAMS, "*", split[0],
			"Not enough parameters");
	}
	else
	{
		if (split[4][0] == ':')
		{
			_realname = split[4].substr(1);
			size_t  i = 4;
			while (++i < split.size())
			{
				_realname += " ";
				_realname += split[i];
			}
		}
		else
			_realname = split[4];
		it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
		if (it != serv->clients.end())
		{
			if (it->Register(split[1], _realname))
			{
				if (it->getNICK())
				{
					nick_forward(serv, &(*it));
					reply = reply_welcome(serv, &(*it));
					serv->fds[fd].linkname = it->getnick() + "[" +
						it->getusername() + "@" + it->gethostname() + "]";
				}
			}
			else
				reply = ft_buildmsg(serv->servername, ERR_ALREADYREGISTRED,
					it->getnick(), "", "You may not reregister");
		}
		else
			serv->clients.push_back(Client(split[1], _realname, serv->token, fd));
	}
	serv->fds[fd].wrbuf += reply;
}
