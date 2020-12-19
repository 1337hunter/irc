/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:39:08 by salec             #+#    #+#             */
/*   Updated: 2020/12/19 13:04:04 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"
#include "tools.hpp"

//split: :<behind> SERVER <servername> <hopcount> <token> <info>
void	introduce_server_behind(int fd, const t_strvect &split, IRCserv *serv)
{
	std::vector<t_server>::iterator begin = serv->network.begin();
	t_server_intro	temp;
	std::string		behind(split[0], 1);
	t_strvect		forward_vect;
	std::string		forward;

	while (begin != serv->network.end() && begin->fd != fd)
		begin++;
	if (begin == serv->network.end()) //error
		return ;
	try { temp.hopcount = stoi(split[3]); } catch (std::exception &e) { (void)e; return; }
	temp.servername = split[2];
	temp.behind = behind;
	temp.info = strvect_to_string(split, ' ', 5);
	temp.token = split[3];
	begin->routing.push_back(temp);
	// forward broadcast
	forward_vect = split;
	forward_vect[3] = std::to_string(temp.hopcount + 1);
	forward = strvect_to_string(forward_vect);
	forward += CRLF;
	begin = serv->network.begin();
	while (begin != serv->network.end())
	{
		if (begin->fd != fd)
			serv->fds[begin->fd].wrbuf += forward;
		begin++;
	}
}

bool	send_clients(int fd, IRCserv *serv)
{
	std::list<Client>::iterator		client;
	std::vector<t_server>::iterator	net;

	for (client = serv->clients.begin(); client != serv->clients.end(); client++)
	{
		serv->fds[fd].wrbuf += "NICK " + client->getnickname() + " 1 " +
			client->getusername() + " " + client->gethostname() + " " +
			serv->token + client->getMode(true) + ":" + client->getrealname() + CRLF;
	}
	for (net = serv->network.begin(); net != serv->network.end(); net++)
	{
		for (client = net->clients.begin(); client != net->clients.end(); client++)
		{
			serv->fds[fd].wrbuf += "NICK " + client->getnickname() +
				client->gethopcount(true, true) + client->getusername() + " " +
				client->gethostname() +
				" " + client->gettoken() + client->getMode(true) +
				":" + client->getrealname() + CRLF;
		}
	}
	return false;
}

//split: SERVER <servername> <hopcount> <token> <info>
void	cmd_server(int fd, const t_strvect &split, IRCserv *serv)
{
	t_server	temp;
	std::vector<t_link>::iterator	link;

	if ((split.size() < 5 && split[0] == "SERVER") ||
			(split.size() < 6 && split[0][0] == ':'))
	{
		serv->fds[fd].wrbuf += "ERROR :Not enough SERVER parameters";
		serv->fds[fd].wrbuf += CRLF;
		serv->fds[fd].status = false;
		return ;
	}
	if (split[0][0] == ':')
	{
		introduce_server_behind(fd, split, serv);
		return ;
	}
	link = serv->link.begin();
	// looking for link with servername (is SERVER servername allowd to connect to us?)
	while (link != serv->link.end())
	{
		if (link->servername == split[1])
			break ;
		link++;
	}
	if (link == serv->link.end())
	{
		serv->fds[fd].wrbuf += "ERROR :You are not allowed to connect to.";
		serv->fds[fd].wrbuf += CRLF;
		serv->fds[fd].status = false;
		return ;
	}
	if (is_server_registred(split[1], serv))
	{
		serv->fds[fd].wrbuf += get_reply(serv, ERR_ALREADYREGISTRED, -1, "",
		split[1] + " :server already registred");
		serv->fds[fd].status = false;
		return ;
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
	//backward message to introduce us to enother server
	if (serv->fds[fd].type != FD_SERVER)
	{
		serv->fds[fd].wrbuf += "PASS " + link->pass + " " + VERSION +
			" " + "IRC|" + CRLF;
		serv->fds[fd].wrbuf += "SERVER " + serv->servername + " 1 " + serv->token +
		" :" + serv->info + CRLF;
	}
	temp.servername = split[1];
	try { temp.hopcount = stoi(split[2]); temp.token = split[3]; }
	catch (std::exception &e)
	{
		msg_error("Bad hopcount. Connection is terminated.", serv);
		cmd_squit(fd, split, serv);
		return ;
	}
	if (temp.hopcount == 1)
		temp.fd = fd;
	else
	{
		msg_error("Error while SERVER message process. Connection Terminated.", serv);
		cmd_squit(fd, split, serv);
		return ;
	}
	temp.info = split[4];
	for (size_t i = 5; i < split.size(); i++)
	{
		temp.info += " ";
		temp.info += split[i];
	}
	std::string	forward;
	forward = ":" + split[1] + " SERVER " + serv->servername + " 2 " + split[3];
	forward += " ";
	forward += temp.info;
	forward += CRLF;
	std::vector<t_server>::iterator begin = serv->network.begin();
	std::vector<t_server>::iterator end = serv->network.end();
	// server introduction (forward message) to the rest of network (here is for nearest)
	while (begin != end)
	{
		serv->fds[begin->fd].wrbuf += forward;
		begin++;
	}
	//server introduction (backward message)
	std::string	backward;
	std::list<t_server_intro>::iterator	serv_intro;
	begin = serv->network.begin();
	while (begin != end)
	{
		backward += ":" + begin->servername + " SERVER " + serv->servername +
			" 2 " + begin->token + " " + begin->info + CRLF; //send nearest servers
		serv_intro = begin->routing.begin();
		while (serv_intro != begin->routing.end())
		{
			backward += ":" + serv_intro->behind + " SERVER " + serv_intro->servername +
				" " + std::to_string(serv_intro->hopcount + 1) + " " +
				serv_intro->token + " " + serv_intro->info + CRLF;
			serv_intro++;
		}
		begin++;
	}
	send_clients(fd, serv);
	serv->fds[fd].wrbuf += backward;
	serv->fds[fd].type = FD_SERVER;
	serv->network.push_back(temp);
}
