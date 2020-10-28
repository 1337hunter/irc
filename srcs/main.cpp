/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 17:03:45 by salec             #+#    #+#             */
/*   Updated: 2020/10/28 18:27:08 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "error_handle.hpp"
#include "commands.hpp"
#include "tools.hpp"

void	server_init(IRCserv *_server, int ac, char **av)
{
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
			t_strvect	temp;

			temp = ft_splitstring(av[1], ":");
			if (temp.size() < 2 || temp.size() > 3)
				error_exit("Error: input parameters of server which you want to connect to is wrong!");
			_server->connected_to_host.push_back(temp[0]);
			if (temp[1].find_first_not_of("0123456789") != std::string::npos ||
                                        temp.size() < 1 || temp.size() > 5)
                                error_exit("Error: bad port number");
			_server->connected_to_port.push_back(stoi(temp[1]));
			if (temp.size() == 3)
				_server->connected_to_pass.push_back(temp[2]);
			else
				_server->connected_to_pass.push_back("");
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
		std::string	port(av[2]);
		std::string	pass(av[3]);
		t_strvect	connect_to;

		connect_to = ft_splitstring(av[1], ":");
		if (connect_to.size() < 2 || connect_to.size() > 3)
			error_exit("Error: input parameters of server which you want to connect to is wrong!");
		_server->connected_to_host.push_back(connect_to[0]);
		_server->pass = pass;
		if (port.find_first_not_of("0123456789") != std::string::npos ||
		connect_to[1].find_first_not_of("0123456789") != std::string::npos ||
		connect_to[1].length() < 1 || connect_to[1].length() > 5 ||
		port.length() < 1 || port.length() > 5)
			error_exit("Error: bad port number");
		else
		{
			_server->connected_to_port.push_back(stoi(connect_to[1]));
			if (connect_to.size() == 3)
				_server->connected_to_pass.push_back(connect_to[2]);
			else
				_server->connected_to_pass.push_back("");
			_server->port = stoi(port);
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
	_server->command["USER"] = cmd_user;
	_server->command["NICK"] = cmd_nick;
	_server->command["PING"] = cmd_ping;
}

void	print_server_info(IRCserv *_server)
{
	std::cout << "server starts with parameters:\n";
	std::cout << "server port:      \t" << _server->port << '\n';
	std::cout << "server pass:      \t" << _server->pass << '\n';
	std::cout << "connected_to_host:\t" << _server->connected_to_host[0] << '\n';
	std::cout << "connected_to_port:\t" << _server->connected_to_port[0] << '\n';
	std::cout << "connected_to_pass:\t" << _server->connected_to_pass[0] << '\n';
}

int		main(int ac, char **av)
{
	IRCserv	_server;

	server_init(&_server, ac, av);
	print_server_info(&_server);
	RunServer(&_server);
	return (0);
}
