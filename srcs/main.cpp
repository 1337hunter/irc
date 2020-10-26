/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 17:03:45 by salec             #+#    #+#             */
/*   Updated: 2020/10/26 23:06:39 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "error_handle.hpp"

void	server_init(IRCserv *_server, int ac, char **av)
{
	_server->fds = NULL;
	if (ac == 2)
	{
		std::string	port(av[1]);

		if (port.find_first_not_of("0123456789") != std::string::npos ||
				port.length() < 1 || port.length() > 5)
			error_exit("Error: bad port number");
		_server->port = stoi(port);
		if (_server->port < 1 || _server->port > 65535)
			error_exit("Error: bad port number");
	}
	else if (ac == 3)
	{
		std::string	port(av[1]);
		std::string	pass(av[2]);

		if (port.find_first_not_of("0123456789") == std::string::npos)
		{
			if (port.length() < 1 || port.length() > 5)
				error_exit("Error: bad port number");
			else
			{
				_server->pass = pass;
				_server->port = stoi(port);
				if (_server->port < 1 || _server->port > 65535)
					error_exit("Error: bad port number");
			}
		}
		else
		{
			_server->server = port;
			if (pass.find_first_not_of("0123456789") != std::string::npos ||
					pass.length() < 1 || pass.length() > 5)
				error_exit("Error: bad port number");
			else
			{
				_server->port = stoi(pass);
				if (_server->port < 1 || _server->port > 65535)
					error_exit("Error: bad port number");
			}
		}
	}
	else if (ac == 4)
	{
		std::string	server(av[1]);
		std::string	port(av[2]);
		std::string	pass(av[3]);

		_server->server = server;
		_server->pass = pass;
		if (port.find_first_not_of("0123456789") != std::string::npos ||
				port.length() < 1 || port.length() > 5)
			error_exit("Error: bad port number");
		else
		{
			_server->port = stoi(pass);
			if (_server->port < 1 || _server->port > 65535)
				error_exit("Error: bad port number");
		}
	}
	else
	{
		std::cerr << "Usage: " << av[0] << " [host:port_network:" <<
			"password_network] <port> <password>" << std::endl;
		exit(1);
	}
	_server->fds = new int[FD_MAX];
}

int		main(int ac, char **av)
{
	IRCserv	server;

	server_init(&server, ac, av);
	server.RunServer();
	return (0);
}
