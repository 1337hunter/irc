/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_user.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:30:28 by salec             #+#    #+#             */
/*   Updated: 2020/11/10 22:14:38 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

#if 0

std::string		success_reply(IRCserv *serv, t_citer it)
{
	std::string	reply;

	reply += ft_buildmsg(serv->hostname, RPL_WELCOME,
			it->getnickname(), "",
			"Welcome to the Internet Relay Network " +
			it->getnickname() + "!" + it->getnickname() + "@" +
			serv->hostname);
	reply += ft_buildmsg(serv->hostname, RPL_YOURHOST,
			it->getnickname(), "", "Your host is " + serv->hostname +
			", running version " + /*serv->version*/ "0.1");
	reply += ft_buildmsg(serv->hostname, RPL_CREATED,
				it->getnickname(), "", "This server was created <date>");
	reply += ft_buildmsg(serv->hostname, RPL_MYINFO,
				it->getnickname(), "", serv->hostname + " " +
				/*serv->version*/ "0.1" + " " +
				"<available user modes>" + " " +
				"<available channel modes>");
	return reply;

}

#endif
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
				std::string realname(split[4], 1);
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
			{
				//if (it->do_reply)
				//	reply = success_reply(serv, it);
				reply += ft_buildmsg(serv->hostname, RPL_WELCOME,
					it->getnickname(), "",
					"Welcome to the Internet Relay Network " +
					it->getnickname() + "!" + it->getnickname() + "@" +
					serv->hostname);
				reply += ft_buildmsg(serv->hostname, RPL_YOURHOST,
					it->getnickname(), "", "Your host is " + serv->hostname +
					", running version " + /*serv->version*/ "0.1");
				reply += ft_buildmsg(serv->hostname, RPL_CREATED,
					it->getnickname(), "", "This server was created <date>");
				reply += ft_buildmsg(serv->hostname, RPL_MYINFO,
					it->getnickname(), "", serv->hostname + " " +
					/*serv->version*/ "0.1" + " " +
					"<available user modes>" + " " +
					"<available channel modes>");
			}
			else
				reply = ft_buildmsg(serv->hostname, ERR_ALREADYREGISTRED,
					it->getnickname(), "", "You may not reregister");
		}
	}
	serv->fds[fd].wrbuf += reply;
}
