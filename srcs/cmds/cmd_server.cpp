/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:39:08 by salec             #+#    #+#             */
/*   Updated: 2020/11/06 20:31:00 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"

void		cmd_server(int fd, const t_strvect &split, IRCserv *_server)
{
	fd_set		fd_send;
	int			n_ready;
	t_server	temp;

	FD_ZERO(&(fd_send));
	FD_SET(fd, &(fd_send));
#if DEBUG_MODE
	if (split.size() > 2)
		std::cout << "incoming connection from:\t" << split[1] << std::endl;
#endif
	if (split.size() < 5) // is info parameter counts?
	{
		std::string	reply;
		reply = "ERROR :Not enough SERVER parameters";
		reply += CLRF;
		n_ready = select(fd + 1, 0, &fd_send, 0, 0);
		if (n_ready < 0)
			error_exit("Error: select system call error");
		send(fd, reply.c_str(), reply.length(), 0);
		return ;
	}
	std::vector<t_server>::iterator	begin = _server->connect.begin();
	std::vector<t_server>::iterator	end = _server->connect.end();
	while (begin != end) //looking for servers with the same name
	{
		if (begin->hostname == split[1])
		{
			std::string	reply;
			reply = ":" + _server->hostname + " ";
			reply += ERR_ALREADYREGISTRED;
			reply += " " + split[1];
			reply += " :server already registred";
			reply += CLRF;
			send(fd, reply.c_str(), reply.length(), 0);
			cmd_squit(fd, split, _server);
			break ;

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
	temp.hostname = split[1];
	try { temp.hopcount = stoi(split[2]); temp.token = split[3]; }
	catch (std::exception &e)
	{
#if DEBUG_MODE
		std::cerr << "Error: bad cast hopcount. Connection is terminated.";
		std::cerr << std::endl;
		cmd_squit(fd, split, _server);
#endif
		return ;
	}
	temp.info = split[4];
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
	std::map<int, t_fd>::iterator	mbegin = _server->fds.begin();
	std::map<int, t_fd>::iterator	mend = _server->fds.end();
	while (mbegin != mend)
	{
		if (mbegin->second.type == FD_SERVER && mbegin->first != _server->sock)
			send(mbegin->first, broadcast.c_str(), broadcast.length(), 0);
		mbegin++;
	}
	_server->fds[fd].type = FD_SERVER;
	_server->connect.push_back(temp);
}
