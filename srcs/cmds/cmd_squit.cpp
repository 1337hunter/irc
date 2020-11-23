/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_squit.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:37:57 by salec             #+#    #+#             */
/*   Updated: 2020/11/23 16:05:38 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

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
	std::map<int, t_fd>::iterator	mit = serv->fds.begin();
	while (begin != end && begin->servername != split[1])
		begin++;
	if (begin != end && begin->fd != -1)
	{
		t_strvect::iterator 		sit = begin->routing.begin();
		std::string	splited_servers;
	
		while (sit != begin->routing.end())
		{
			splited_servers += "SQUIT " + *sit + " :Network split" + CRLF;
			sit++;
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
	else if (begin != end && begin->fd == -1)
	{
		std::string recovered_split = strvect_to_string(split);

		while (mit != serv->fds.end())
		{
			if (mit->second.type == FD_SERVER && mit->first != fd)
				serv->fds[mit->first].wrbuf += recovered_split;
			mit++;
		}
		serv->network.erase(begin);
	}
	else
		serv->fds[fd].wrbuf += get_reply(serv, ERR_NOSUCHSERVER, fd, "",
				"No such server");
}
