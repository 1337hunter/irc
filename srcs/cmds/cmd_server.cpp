/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_server.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 16:39:08 by salec             #+#    #+#             */
/*   Updated: 2020/11/25 17:43:05 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "commands.hpp"
#include "message.hpp"

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

//Command: SERVER
//Parameters: <servername> <hopcount> <token> <info>
void	cmd_server(int fd, const t_strvect &split, IRCserv *serv)
{
	t_server	temp;

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
	forward = ":" + split[1] + " SERVER " + serv->servername + " " + " 2 " + split[4];
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
				serv_intro->token + " " + serv_intro->info;
			serv_intro++;
		}
		begin++;
	}
	serv->fds[fd].wrbuf += backward;
	serv->fds[fd].type = FD_SERVER;
	serv->network.push_back(temp);
}
