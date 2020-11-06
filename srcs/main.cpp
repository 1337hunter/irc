/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 17:03:45 by salec             #+#    #+#             */
/*   Updated: 2020/11/06 18:48:21 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "error_handle.hpp"
#include "commands.hpp"
#include "tools.hpp"

#if DEBUG_MODE
void	print_server_info(IRCserv *_server)
{
	std::cout << "server starts with parameters:\n";
	std::cout << "server port:      \t" << _server->port << '\n';
	std::cout << "server pass:      \t" << _server->pass << '\n';
	if (_server->connect.size() > 0)
	{
		std::cout << "connected to host:\t" << _server->connect[0].hostname << '\n';
		std::cout << "connected to port:\t" << _server->connect[0].port << '\n';
		std::cout << "connected to pass:\t" << _server->connect[0].pass << '\n';
	}
}
#endif

int		main(int ac, char **av)
{
	IRCserv	_server;

	parse(ac, av, &_server);
	_server.hostname = "irc.sparta.net";
#if DEBUG_MODE
	print_server_info(&_server);
#endif
	RunServer(&_server);
	return (0);
}
