/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:39:08 by salec             #+#    #+#             */
/*   Updated: 2020/11/13 14:04:37 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"

void		cmd_server(int fd, const t_strvect &split, IRCserv *_server)
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
		reply += CLRF;
		_server->fds[fd].wrbuf += reply;
		_server->fds[fd].status = false;
		return ;
	}
	std::vector<t_server>::iterator	begin = _server->connect.begin();
	std::vector<t_server>::iterator	end = _server->connect.end();
	while (begin != end) //looking for servers with the same name
	{
		if (begin->hostname == split[1])
		{
			_server->fds[fd].wrbuf += ":" + _server->hostname + " ";
			_server->fds[fd].wrbuf += ERR_ALREADYREGISTRED;
			_server->fds[fd].wrbuf += " " + split[1];
			_server->fds[fd].wrbuf += " :server already registred";
			_server->fds[fd].wrbuf += CLRF;
			_server->fds[fd].status = false;
			return ;

		}
		begin++;
	}
	if (!(_server->fds[fd].pass == _server->pass || _server->pass == ""))
	{
#if DEBUG_MODE
		std::cout << "client " << fd << "\t\twrong server password" << std::endl;
#endif
		cmd_squit(fd, split, _server);
		return ;
	}
	temp.fd = fd;
	temp.hostname = split[1];
	try { temp.hopcount = stoi(split[2]); temp.token = split[3]; }
	catch (std::exception &e)
	{
		msg_error("Error: bad cast hopcount. Connection is terminated.", _server);
		cmd_squit(fd, split, _server);
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
	broadcast += CLRF;
	std::map<int, t_fd>::iterator	b = _server->fds.begin();
	std::map<int, t_fd>::iterator	e = _server->fds.end();
	while (b != e)
	{
		if (b->second.type == FD_SERVER)
			b->second.wrbuf += broadcast;
		b++;
	}
	_server->fds[fd].type = FD_SERVER;
	_server->connect.push_back(temp);
}
