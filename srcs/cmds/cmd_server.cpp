/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:39:08 by salec             #+#    #+#             */
/*   Updated: 2020/11/13 17:02:01 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"

void		cmd_server(int fd, const t_strvect &split, IRCserv *serv)
{
	t_server	temp;

#if DEBUG_MODE
	if (split.size() > 2)
		std::cout << "incoming connection:\t" << split[1] << std::endl;
#endif
	if (split.size() < 5) // is info parameter counts?
	{
		std::string	reply;
		reply = "ERROR :Not enough SERVER parameters";
		reply += CRLF;
		serv->fds[fd].wrbuf += reply;
		serv->fds[fd].status = false;
		return ;
	}
	std::vector<t_server>::iterator	begin = serv->connect.begin();
	std::vector<t_server>::iterator	end = serv->connect.end();
	while (begin != end) //looking for servers with the same name
	{
		if (begin->hostname == split[1])
		{
			serv->fds[fd].wrbuf += ":" + serv->hostname + " ";
			serv->fds[fd].wrbuf += ERR_ALREADYREGISTRED;
			serv->fds[fd].wrbuf += " " + split[1];
			serv->fds[fd].wrbuf += " :server already registred";
			serv->fds[fd].wrbuf += CRLF;
			serv->fds[fd].status = false;
			return ;

		}
		begin++;
	}
	if (!(serv->fds[fd].pass == serv->pass || serv->pass == ""))
	{
#if DEBUG_MODE
		std::cout << "client " << fd << "\t\twrong server password" << std::endl;
#endif
		cmd_squit(fd, split, serv);
		return ;
	}
	temp.fd = fd;
	temp.hostname = split[1];
	try { temp.hopcount = stoi(split[2]); temp.token = split[3]; }
	catch (std::exception &e)
	{
		msg_error("Error: bad cast hopcount. Connection is terminated.", serv);
		cmd_squit(fd, split, serv);
		return ;
	}
	temp.info = split[4];
	for (size_t i = 5; i < split.size(); i++)
	{
		temp.info += " ";
		temp.info += split[i];
	}
	std::string	broadcast;
	std::vector<std::string>::const_iterator	itbegin = split.begin();
	std::vector<std::string>::const_iterator	itend = split.end();
	size_t	i = 0;
	while (itbegin != itend)
	{
		if (i != 2)
			broadcast += *itbegin;
		else
			broadcast += std::to_string(temp.hopcount + 1);
		if (itbegin + 1 != itend)
			broadcast += " ";
		itbegin++;
		i++;
	}
	broadcast += CRLF;
	std::map<int, t_fd>::iterator	b = serv->fds.begin();
	std::map<int, t_fd>::iterator	e = serv->fds.end();
	while (b != e)
	{
		if (b->second.type == FD_SERVER)
			b->second.wrbuf += broadcast;
		b++;
	}
	serv->fds[fd].type = FD_SERVER;
	serv->connect.push_back(temp);
}
