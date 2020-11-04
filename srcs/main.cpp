/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 17:03:45 by salec             #+#    #+#             */
/*   Updated: 2020/11/04 18:18:24 by salec            ###   ########.fr       */
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
			server_server	connect;

			temp = ft_splitstring(av[1], ":");
			if (temp.size() < 2 || temp.size() > 3)
				error_exit("Error: input parameters of server which you want to connect to is wrong!");
			connect.host = temp[0];
			if (temp[1].find_first_not_of("0123456789") != std::string::npos ||
                                        temp.size() < 1 || temp.size() > 5)
                                error_exit("Error: bad port number");
			connect.port = stoi(temp[1]);
			if (temp.size() == 3)
				connect.pass = temp[2];
			if (pass.find_first_not_of("0123456789") != std::string::npos ||
					pass.length() < 1 || pass.length() > 5)
				error_exit("Error: bad port number");
			else
			{
				_server->port = stoi(pass);
				if (_server->port < 1 || _server->port > 65535)
					error_exit("Error: bad port number");
			}
			_server->connect.push_back(connect);
		}
	}
	else if (ac == 4)
	{
		std::string	port(av[2]);
		std::string	pass(av[3]);
		t_strvect	connect_to;
		server_server	temp;

		connect_to = ft_splitstring(av[1], ":");
		if (connect_to.size() < 2 || connect_to.size() > 3)
			error_exit("Error: input parameters of server which you want to connect to is wrong!");
		temp.type = TO;
		temp.host = connect_to[0];
		_server->pass = pass;
		if (port.find_first_not_of("0123456789") != std::string::npos ||
		connect_to[1].find_first_not_of("0123456789") != std::string::npos ||
		connect_to[1].length() < 1 || connect_to[1].length() > 5 ||
		port.length() < 1 || port.length() > 5)
			error_exit("Error: bad port number");
		else
		{
			temp.port = stoi(connect_to[1]);
			if (connect_to.size() == 3)
				temp.pass = connect_to[2];
			_server->port = stoi(port);
			if (_server->port < 1 || _server->port > 65535)
				error_exit("Error: bad port number");
			_server->connect.push_back(temp);
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
	_server->command["QUIT"] = cmd_quit;
	_server->command["PASS"] = cmd_pass;
	_server->command["SERVER"] = cmd_server;
	_server->command["SQUIT"] = cmd_squit;
	_server->command["CONNECT"] = cmd_connect;
}

#if DEBUG_MODE
void	print_server_info(IRCserv *_server)
{
	std::cout << "server starts with parameters:\n";
	std::cout << "server port:      \t" << _server->port << '\n';
	std::cout << "server pass:      \t" << _server->pass << '\n';
	if (_server->connect.size() > 0)
	{
		std::cout << "connected to host:\t" << _server->connect[0].host << '\n';
		std::cout << "connected to port:\t" << _server->connect[0].port << '\n';
		std::cout << "connected to pass:\t" << _server->connect[0].pass << '\n';
	}
}
#endif

int		main(int ac, char **av)
{
	IRCserv	_server;

	server_init(&_server, ac, av);
	_server.hostname = "irc.sparta.net";
#if DEBUG_MODE
	print_server_info(&_server);
#endif
	RunServer(&_server);
	return (0);
}
