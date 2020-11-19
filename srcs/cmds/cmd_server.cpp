/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:39:08 by salec             #+#    #+#             */
/*   Updated: 2020/11/19 13:22:17 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"

//Command: SERVER
//Parameters: <servername> <hopcount> <info>
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
	std::vector<t_server>::iterator	begin = serv->network.begin();
	std::vector<t_server>::iterator	end = serv->network.end();
	while (begin != end) //looking for servers with the same name
	{
		if (begin->servername == split[1])
		{
			serv->fds[fd].wrbuf += ":" + serv->servername + " ";
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
		serv->fds[fd].wrbuf += "ERROR :Password incorrect";
		serv->fds[fd].status = false;
		return ;
	}
	temp.fd = fd;
	temp.servername = split[1];
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
	serv->network.push_back(temp);
}
