/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 16:44:15 by salec             #+#    #+#             */
/*   Updated: 2020/11/13 20:45:27 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

std::string const	IRCserv::clrf = CRLF;

void		CreateSock(IRCserv *serv)
{
	t_sockaddr_in	sockin;
	t_protoent		*pe = NULL;
	int				optval = 1;

	if (!(pe = getprotobyname("tcp")))
		error_exit("getprotobyname error");
	if ((serv->sock = socket(PF_INET, SOCK_STREAM, pe->p_proto)) < 0)
		error_exit("socket error");
	if (fcntl(serv->sock, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	sockin.sin_family = AF_INET;
	sockin.sin_addr.s_addr = /*INADDR_ANY;*/ inet_addr("127.0.0.1");
	//	can change ip to create another local server without the b8s or
	//	containers just with ip variable seted in config file
	sockin.sin_port = htons(serv->port);
	if (setsockopt(serv->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)))
		error_exit("set socket option returned error");
	if (bind(serv->sock, (t_sockaddr*)&sockin, sizeof(sockin)) < 0)
		error_exit("bind error (probably already binded)");
	if (listen(serv->sock, 42) < 0)
		error_exit("listen error");
	serv->fds[serv->sock].type = FD_ME;
	std::cout << "server created on socket " << serv->sock << std::endl;
}

void		AcceptConnect(IRCserv *serv)
{
	int				fd;
	t_sockaddr_in	csin;
	socklen_t		csin_len;

	csin_len = sizeof(csin);
	if ((fd = accept(serv->sock, (t_sockaddr*)&csin, &csin_len)) < 0)
		error_exit("accept error");
	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
		error_exit("fcntl error: failed to set nonblock fd");
	std::cout << "client " << fd << " accepted from\t" <<
		inet_ntoa(csin.sin_addr) << ":" << ntohs(csin.sin_port) << std::endl;
	serv->fds[fd].type = FD_CLIENT;
	// we dont know either it's client or other server
	serv->fds[fd].rdbuf.erase();
	serv->fds[fd].wrbuf.erase();
	serv->fds[fd].status = true;
}

void		ProcessMessage(int fd, std::string const &msg, IRCserv *serv)
{
	t_strvect	split = ft_splitstring(msg, " ");

	try
	{
		serv->command.at(split[0])(fd, split, serv);
	#if DEBUG_MODE
		std::cout << "command found:\t\t" << split[0] << std::endl;
	#endif
	}
	catch (std::out_of_range &e) { (void)e; }
}

void		ReceiveMessage(int fd, IRCserv *serv)
{
	ssize_t		r;
	char		buf_read[BUF_SIZE + 1];

	if ((r = recv(fd, buf_read, BUF_SIZE, 0)) >= 0)
		buf_read[r] = 0;
	if (r > 0)
	{
		serv->fds[fd].rdbuf += buf_read;
		if (serv->fds[fd].rdbuf.rfind(CRLF) + serv->clrf.length() ==
			serv->fds[fd].rdbuf.length())
		{
#if DEBUG_MODE
			std::cout << "client " << fd << " sent:\t\t" << serv->fds[fd].rdbuf;
#endif
			t_strvect	split = ft_splitstring(serv->fds[fd].rdbuf, CRLF);
			for (size_t i = 0; i < split.size(); i++)
				ProcessMessage(fd, split[i], serv);
			try { serv->fds.at(fd).rdbuf.erase(); }
			catch (std::out_of_range const &e) { (void)e; }
		}
	}
	else
	{
		close(fd);
		serv->fds.erase(fd);
		t_citer	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
		if (it != serv->clients.end())
			it->Disconnect();
		std::cout << "client " << fd << "\t\tdisconnected" << std::endl;
	}
}

void		SendMessage(int fd, IRCserv *serv)
{
	std::string	reply;

	if (serv->fds[fd].wrbuf.length() > BUF_SIZE)
	{
		reply = serv->fds[fd].wrbuf.substr(0, BUF_SIZE);
		serv->fds[fd].wrbuf = serv->fds[fd].wrbuf.substr(BUF_SIZE);
	}
	else
	{
		reply = serv->fds[fd].wrbuf;
		serv->fds[fd].wrbuf.erase();
	}
#if DEBUG_MODE
	std::cout << "sending client " << fd << "\t" << reply;
#endif
	ssize_t		r = send(fd, reply.c_str(), reply.length(), 0);
	if (r <= 0 || serv->fds[fd].status == false)
	{
		close(fd);
		serv->fds.erase(fd);
		t_citer	it = ft_findclientfd(serv->clients.begin(), serv->clients.end(), fd);
		if (it != serv->clients.end())
			it->Disconnect();
		std::cout << "client " << fd << ":\t\tdisconnected" << std::endl;
	}
}

void		RunServer(IRCserv *serv)
{
	CreateSock(serv);
	while (1)
	{
		int	lastfd = 0;
		FD_ZERO(&(serv->fdset_read));
		FD_ZERO(&(serv->fdset_write));
		FD_ZERO(&(serv->fdset_error));
		for (std::map<int, t_fd>::iterator it = serv->fds.begin();
			it != serv->fds.end(); it++)
		{
			FD_SET(it->first, &(serv->fdset_read));
			if (!(it->second.wrbuf.empty()))
				FD_SET(it->first, &(serv->fdset_write));
			lastfd = std::max(lastfd, it->first);
		}
		int readyfds = select(lastfd + 1,
			&(serv->fdset_read), &(serv->fdset_write),
			&(serv->fdset_error), NULL);
		if (readyfds < 0)
			error_exit(SELECT);
		for (int fd = 0; readyfds > 0 && fd <= lastfd; fd++)
		{
			bool	isread = FD_ISSET(fd, &(serv->fdset_read));
			bool	iswrite = FD_ISSET(fd, &(serv->fdset_write));
			bool	iserror = FD_ISSET(fd, &(serv->fdset_error));

			if (isread || iswrite || iserror)
				readyfds--;
			if (iserror)
			{
				std::cerr << "some error happened on fd " << fd << std::endl;
				continue ;
			}
			if (isread)
			{
				if (serv->fds[fd].type == FD_ME && !(serv->fds[fd].tls))
					AcceptConnect(serv);
				else if (serv->fds[fd].type == FD_ME && serv->fds[fd].tls)
					AcceptConnect(serv);		// tls accepter
				else if (!(serv->fds[fd].tls))
					ReceiveMessage(fd, serv);
				else if (serv->fds[fd].tls)
					ReceiveMessage(fd, serv);	// tls reciever	
			}
			if (iswrite)
			{
				if (!serv->fds[fd].tls)
					SendMessage(fd, serv);
				else
					SendMessage(fd, serv);		// tls sender
			}
		}
	}
}
