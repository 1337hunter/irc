/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_squit.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:37:57 by salec             #+#    #+#             */
/*   Updated: 2020/11/25 16:57:29 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

void		remove_introduced_server(int fd, const t_strvect &split, IRCserv *serv)
{
	std::vector<t_server>::iterator serv_in_network = serv->network.begin();
	std::list<t_server_intro>::iterator	serv_intro;

	while (serv_in_network != serv->network.end())
	{
		serv_intro = serv_in_network->routing.begin();
		while (serv_intro != serv_in_network->routing.end())
		{
			if (serv_intro->behind == split[2])
			{
				std::map<int, t_fd>::iterator   mit = serv->fds.begin();
				std::string	forward;

				forward = strvect_to_string(split);
				serv_in_network->routing.erase(serv_intro);
				while (mit != serv->fds.begin())
				{
					if (mit->second.type == FD_SERVER && mit->first != fd)
						serv->fds[mit->first].wrbuf += forward;
					mit++;
				}
			}
		}
	}
}

void		cmd_squit(int fd, const t_strvect &split, IRCserv *serv)
{
	std::vector<t_server>::iterator	begin = serv->network.begin();
	std::vector<t_server>::iterator	end = serv->network.end();

	if ((split[0][0] != ':' && split.size() < 2 ) ||
			(split[0][0] == ':' && split.size() < 3))
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NEEDMOREPARAMS, fd, "SQUIT",
				"Not enough parameters");
		return ;
	}
	if (split[0][0] == ':')
	{
		remove_introduced_server(fd, split, serv);
		return ;
	}
	while (begin != end && begin->servername != split[1])
		begin++;
	if (begin != end)
	{
		std::list<t_server_intro>::iterator	serv_intro = begin->routing.begin();
		std::string	splited_servers;
		std::map<int, t_fd>::iterator   mit = serv->fds.begin();

		while (serv_intro != begin->routing.end())
		{
			splited_servers += ":" + serv->servername + " SQUIT " +
				serv_intro->behind + " :Network split" + CRLF;
			serv_intro++;
		}
		while (mit != serv->fds.end())
		{
			if (mit->second.type == FD_SERVER && mit->first != begin->fd)
				serv->fds[mit->first].wrbuf += splited_servers;
			mit++;
		}
		if (serv->fds[fd].type == FD_OPER)
		{
			serv->fds[begin->fd].wrbuf += "SQUIT " + serv->servername;
			if (split.size() > 2)
				serv->fds[begin->fd].wrbuf += strvect_to_string(split, ' ', 2);
			else
				serv->fds[begin->fd].wrbuf += ":I am out of here!";
			serv->fds[begin->fd].wrbuf += CRLF;
		}
		serv->fds[begin->fd].status = false;
		serv->network.erase(begin);
#if DEBUG_MODE
		std::cout << "server " << split[1] << ":\tdisconnected" << std::endl;
#endif
	}
	else
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHSERVER, fd, "",
				split[1] + " :No such server");
}
