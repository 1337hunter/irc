/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/27 15:24:34 by gbright           #+#    #+#             */
/*   Updated: 2020/10/27 16:36:00 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

t_citer         ft_findclientfd(t_citer const &begin, t_citer const &end, int fd)
{
        for (t_citer it = begin; it != end; it++)
                if (it->isConnected() && it->getFD() == fd)
                        return (it);
        return (end);
}

t_citer         ft_findnick(t_citer const &begin, t_citer const &end, std::string const &nick)
{
        for (t_citer it = begin; it != end; it++)
                if (it->getnickname() == nick)
                        return (it);
        return (end);
}

void	cmd_nick(int fd, const t_strvect &split, IRCserv *_server)
{
	std::string             reply;

	if (ft_findnick(_server->clients.begin(), _server->clients.end(), split[1]) == _server->clients.end())
		_server->clients.push_back(Client(split[1], fd));
	else
	{
		reply = ":localhost ";
		reply += ERR_NICKNAMEINUSE;
		reply += " " + split[1] + " :Nickname is already in use" + CLRF;
		send(fd, reply.c_str(), reply.length(), 0);
		/*      need to save the state in this case
		 *      cli will try to send another NICK after USER msg */

	}
}

void	cmd_user(int fd, const t_strvect &split, IRCserv *_server)
{
	std::string             reply;
	t_citer                 it;

	it = ft_findclientfd(_server->clients.begin(), _server->clients.end(), fd);
	if (it != _server->clients.end())
	{
		it->Register(split[1], split[4]);
		reply = ":localhost ";
		reply += RPL_WELCOME;
		reply += " " + it->getnickname() + " :Welcome to the Internet Relay Network " + it->getnickname() + "!" + it->getnickname() + "@" + "localhost" + CLRF;
		send(fd, reply.c_str(), reply.length(), 0);
	}

}

void	cmd_ping(int fd, const t_strvect &split, IRCserv *_server)
{
	std::string             reply;

	_server = 0;
	reply = "PONG " + split[1] + CLRF;
	send(fd, reply.c_str(), reply.length(), 0);
}
