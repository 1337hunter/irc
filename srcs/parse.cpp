/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 15:56:53 by gbright           #+#    #+#             */
/*   Updated: 2020/11/06 18:51:40 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * There is a lot of magic in this place!
*/

#include "error_handle.hpp"
#include "ircserv.hpp"
#include "commands.hpp"
#include "tools.hpp"
#include <string>
#include <iostream>
#include <fstream>

#define LISTEN 0
#define LINK 1
#define ADMIN 2
#define OPER 3
#define ME 4

typedef	int (*t_block)(std::fstream &config, std::string &line, IRCserv *_server, size_t &line_number);
typedef std::map<int, t_block> t_blockmap;

size_t	ft_strlen(const char *s)
{
	size_t	c;

	c = 0;
	if (!s)
		return (0);
	while (s[c])
		c++;
	return c;
}

int
block_listen(std::fstream &config, std::string &line, IRCserv *_server, size_t &line_number)
{
	size_t	pos;
	size_t	pos_copy_from;
	size_t	pos_copy_size;
	size_t	i;
	t_listen	temp;

	temp.ssl = false;
	temp.tls = false;
	temp.serveronly = false;
	pos = line.find_first_not_of(" \t");
	pos += ft_strlen("listen");
	while ((pos = line.find_first_not_of(" \t\n", pos)) == std::string::npos || line.c_str()[pos] == '#')
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line.c_str()[pos] != '{' || config.eof())
		return -1;
	pos++;
	while (line.c_str()[pos] != '}')
	{
		while ((pos = line.find_first_not_of(" \t\n", pos)) == std::string::npos)
		{
			getline(config, line);
			line_number++;
			if (config.eof())
				return -1;
			pos = 0;
		}
		if (!line.compare(pos, 2, "ip"))
		{
			pos = line.find_first_not_of(" \n\t", pos + 2);
			if (pos == std::string::npos)
				return -1;
			i = line.find_first_not_of("0123456789.", pos);
			pos_copy_from = pos;
			pos_copy_size = i - pos;
			if (i == std::string::npos)
				return -1;
			pos = line.find_first_not_of(" \n\t", i);
			if (line.c_str()[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != std::string::npos && line.c_str()[pos] != '#' && line.c_str()[pos] != '}'
					&& line.compare(pos, 4, "port") && line.compare(pos, 7, "options"))
				return -1;
			std::string	ip(line, pos_copy_from, pos_copy_size);
			temp.ip = ip;
		}
		else if (!line.compare(pos, 4, "port"))
		{
			pos = line.find_first_not_of(" \n\t", pos + 4);
			if (pos == std::string::npos)
				return -1;
			i = line.find_first_not_of("0123456789", pos);
			pos_copy_from = pos;
			pos_copy_size = i - pos;
			if (i == std::string::npos)
				return -1;
			pos = line.find_first_not_of(" \n\t", i);
			if (line.c_str()[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != std::string::npos && line.c_str()[pos] != '#' && line.c_str()[pos] != '}' &&
					line.compare(pos, 7, "options") && line.compare(pos, 2, "ip"))
				return -1;
			std::string	port(line, pos_copy_from, pos_copy_size);
			temp.port = std::stoi(port);
		}
		else if (!line.compare(pos, 7, "options"))
		{
			pos = line.find_first_not_of(" \n\t", pos + 7);
			if (line.c_str()[pos] != '{')
				return -1;
			pos = line.find_first_not_of(" \n\t", pos + 1);
			while (line.c_str()[pos] != '}' && pos != std::string::npos)
			{
				if (!line.compare(pos, 3, "ssl"))
				{
					temp.ssl = true;
					if (temp.tls)
						return -1;
					pos = line.find_first_not_of(" \n\t", pos + 3);
					if (line.c_str()[pos] != ';')
						return -1;
					pos++;
				}
				else if (!line.compare(pos, 10, "serveronly"))
				{
					temp.serveronly = true;
					pos = line.find_first_not_of(" \n\t", pos + 10);
					if (line.c_str()[pos] != ';')
						return -1;
					pos++;
				}
				else if (!line.compare(pos, 3, "tls"))
				{
					temp.tls = true;
					if (temp.ssl)
						return -1;
					pos = line.find_first_not_of(" \n\t", pos + 3);
					if (line.c_str()[pos] != ';')
						return -1;
					pos++;
				}
				else
					return -1;
				pos = line.find_first_not_of(" \n\t", pos);
			}
			if (pos == std::string::npos || line.c_str()[pos] != '}')
				return -1;
			pos++;
		}
		else if (!line.compare(pos, 1, "#"))
		{
			pos = std::string::npos;
			continue ;
		}
		else if (!line.compare(pos, 1, "}"))
			break ;
		else
			return -1;
	}
	_server->listen.push_back(temp);
#if DEBUG_MODE
	std::vector<t_listen>::reverse_iterator	it = _server->listen.rbegin();
	std::cout << "listen: ip " << it->ip << " port " << it->port << ' ';
	if (temp.ssl)
		std::cout << "ssl ";
	if (temp.serveronly)
		std::cout << "serveronly ";
	if (temp.tls)
		std::cout << "tls ";
	std::cout << std::endl;
#endif
	return 0;
}

int
block_admin(std::fstream &config, std::string &line, IRCserv *_server, size_t &line_number)
{
	size_t	pos;
	size_t	i;
	size_t	endl = std::string::npos;
	std::vector<std::string>	names;

	if (_server->admin.set)
		return -1;
	_server->admin.set = true;
	pos = line.find_first_not_of(" \t");
	pos += ft_strlen("admin");
	while ((pos = line.find("{", pos)) == std::string::npos)
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line.c_str()[pos] != '{' || config.eof())
		return -1;
	pos++;
	int k = 3;
	while (k)
	{
		if (line.c_str()[pos] != '"')
			while ((pos = line.find_first_not_of(" \t\n", pos)) == std::string::npos)
			{
				getline(config, line);
				line_number++;
				if (config.eof())
					return -1;
				pos = 0;
			}
		if (line.c_str()[pos] == '#' && (pos = endl))
			continue ;
		if (line.c_str()[pos] != '"')
			return -1;
		if ((i = line.find("\"", pos + 1)) == std::string::npos)
			return -1;
		std::string	realname(line, pos + 1, i - pos - 1);
		names.push_back(realname);
		pos = line.find_first_not_of(" \t\n", i + 1);
		if (line.c_str()[pos] != ';')
			return -1;
		pos = line.find_first_not_of(" \t\n", pos + 1);
		if (line.c_str()[pos] != '#' && line.c_str()[pos] != '"' && pos != endl &&
				!(k == 1 && line.c_str()[pos] == '}'))
			return -1;
		if (line.c_str()[pos] == '#')
			pos = endl;
		k--;
	}
	while ((pos = line.find_first_not_of(" \t\n", pos)) == std::string::npos)
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line.c_str()[pos] != '}')
		return -1;
	pos = line.find_first_not_of(" \t\n", pos + 1);
	if (line.c_str()[pos] != '#' && pos != endl)
		return -1;
	_server->admin.realname = names[0];
	_server->admin.nick = names[1];
	_server->admin.mail = names[2];
#if DEBUG_MODE
	std::cout << "admin.realname: \"" << _server->admin.realname << '"';
	std::cout << " admin.nick: \"" << _server->admin.nick << '"';
	std::cout << " admin.mail: \"" << _server->admin.mail << '"' << '\n';
#endif
	return 0;
}

int
block_link(std::fstream &config, std::string &line, IRCserv *_server, size_t &line_number)
{
	t_link	temp;
	size_t	pos;
	size_t	i;
	size_t	pos_copy_from;
	size_t	pos_copy_size;

	temp.ssl = false;
	temp.tls = false;
	temp.autoconnect = false;
	pos = line.find_first_not_of(" \t");
	pos += ft_strlen("link");
	while ((pos = line.find_first_not_of(" \t", pos)) == std::string::npos || line.c_str()[pos] == '#')
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line.c_str()[pos] != '{' || config.eof())
		return -1;
	pos++;
	while (line.c_str()[pos] != '}')
	{
		while ((pos = line.find_first_not_of(" \t", pos)) == std::string::npos)
		{
			getline(config, line);
			line_number++;
			if (config.eof())
				return -1;
			pos = 0;
		}
		if (!line.compare(pos, 2, "ip"))
		{
			pos = line.find_first_not_of(" \t", pos + 2);
			if (pos == std::string::npos)
				return -1;
			i = line.find_first_not_of("0123456789.", pos);
			pos_copy_from = pos;
			pos_copy_size = i - pos;
			if (i == std::string::npos)
				return -1;
			pos = line.find_first_not_of(" \t", i);
			if (line.c_str()[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t", pos + 1);
			if (pos != std::string::npos && line.c_str()[pos] != '#' && line.c_str()[pos] != '}' && line.compare(pos, 8, "hostname") && line.compare(pos, 4, "pass") && line.compare(pos, 7, "options") && line.compare(pos, 4, "port"))
				return -1;
			std::string	ip(line, pos_copy_from, pos_copy_size);
			temp.ip = ip;
		}
		else if (!line.compare(pos, 8, "hostname"))
		{
			pos = line.find_first_not_of(" \t", pos + 8);
			if (pos == std::string::npos)
				return -1;
			i = line.find_first_of(" \t;", pos);
			pos_copy_from = pos;
			pos_copy_size = i - pos;
			if (i == std::string::npos)
				return -1;
			pos = line.find_first_not_of(" \t", i);
			if (line.c_str()[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t", pos + 1);
			 if (pos != std::string::npos && line.c_str()[pos] != '#' && line.c_str()[pos] != '}' && line.compare(pos, 2, "ip") && line.compare(pos, 4, "pass") && line.compare(pos, 7, "options") && line.compare(pos, 4, "port"))
				 return -1;
			 std::string     hostname(line, pos_copy_from, pos_copy_size);
			 temp.hostname = hostname;
		}
		else if (!line.compare(pos, 4, "port"))
		{
			pos = line.find_first_not_of(" \n\t", pos + 4);
			if (pos == std::string::npos)
				return -1;
			i = line.find_first_not_of("0123456789", pos);
			pos_copy_from = pos;
			pos_copy_size = i - pos;
			if (i == std::string::npos)
				return -1;
			pos = line.find_first_not_of(" \n\t", i);
			if (line.c_str()[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t\n", pos + 1);
			if (pos != std::string::npos && line.c_str()[pos] != '#' && line.c_str()[pos] != '}' && line.compare(pos, 2, "ip") && line.compare(pos, 4, "pass") && line.compare(pos, 7, "options") && line.compare(pos, 8, "hostname"))
				return -1;
			std::string     port(line, pos_copy_from, pos_copy_size);
			temp.port = std::stoi(port);
		}
		else if (!line.compare(pos, 4, "pass"))
		{
			pos = line.find_first_not_of(" \t", pos + 4);
			if (pos == std::string::npos || line.c_str()[pos] != '"')
				return -1;
			if ((i = line.find("\"", pos + 1)) == std::string::npos)
				return -1;
			std::string	pass(line, pos + 1, i - pos - 1);
			temp.pass = pass;
			pos = line.find_first_not_of(" \t", i + 1);
			if (line.c_str()[pos] != ';')
				return -1;
			pos = line.find_first_not_of(" \t", pos + 1);
			if (pos != std::string::npos && line.c_str()[pos] != '#' && line.c_str()[pos] != '}' && line.compare(pos, 2, "ip") && line.compare(pos, 4, "port") && line.compare(pos, 7, "options") && line.compare(pos, 8, "hostname"))
				return -1;
		}
		else if (!line.compare(pos, 7, "options"))
		{
			pos = line.find_first_not_of(" \n\t", pos + 7);
			if (line.c_str()[pos] != '{')
				return -1;
			pos = line.find_first_not_of(" \n\t", pos + 1);
			while (line.c_str()[pos] != '}' && pos != std::string::npos)
			{
				if (!line.compare(pos, 3, "ssl"))
				{
					temp.ssl = true;
					if (temp.tls)
						return -1;
					pos = line.find_first_not_of(" \t", pos + 3);
					if (line.c_str()[pos] != ';')
						return -1;
					pos++;
				}
				else if (!line.compare(pos, 3, "tls"))
				{
					temp.tls = true;
					if (temp.ssl)
						return -1;
					pos = line.find_first_not_of(" \t", pos + 3);
					if (line.c_str()[pos] != ';')
						return -1;
					pos++;
				}
				else if (!line.compare(pos, 3, "tls"))
				{
					temp.tls = true;
					if (temp.ssl)
						return -1;
					pos = line.find_first_not_of(" \n\t", pos + 3);
					if (line.c_str()[pos] != ';')
						return -1;
					pos++;
				}
				else
					return -1;
				pos = line.find_first_not_of(" \n\t", pos);
			}
			if (pos == std::string::npos || line.c_str()[pos] != '}')
				return -1;
			pos++;
		}
		else if (!line.compare(pos, 1, "#"))
			pos = std::string::npos;
		else if (!line.compare(pos, 1, "}"))
			break ;
		else
			return -1;
	}
	_server->link.push_back(temp);
#if DEBUG_MODE
	std::vector<t_link>::reverse_iterator it = _server->link.rbegin();
	std::cout << "link: ip '" << it->ip << "' hostname '" << it->hostname << "'";
	std::cout << " port '" << it->port << "'" << " pass '" << it->pass << "' ";
	if (temp.ssl)
                std::cout << "'ssl' ";
        if (temp.autoconnect)
                std::cout << "'autoconnect' ";
        if (temp.tls)
				std::cout << "'tls' ";
		std::cout << std::endl;
#endif
	return 0;
}

int
block_me(std::fstream &config, std::string &line, IRCserv *_server, size_t &line_number)
{
	size_t  pos;
	size_t  i;
	size_t  endl = std::string::npos;
	std::vector<std::string>		names;

	pos = line.find_first_not_of(" \t");
	pos += ft_strlen("me");
	while ((pos = line.find("{", pos)) == std::string::npos)
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line.c_str()[pos] != '{' || config.eof())
		return -1;
	pos++;
	int k = 3;
	while (k)
	{
		if (line.c_str()[pos] != '"')
			while ((pos = line.find_first_not_of(" \t\n", pos)) == std::string::npos)
			{
				getline(config, line);
				line_number++;
				if (config.eof())
					return -1;
				pos = 0;
			}
		if (line.c_str()[pos] == '#' && (pos = endl))
			continue ;
		if (line.c_str()[pos] != '"')
			return -1;
		if ((i = line.find("\"", pos + 1)) == std::string::npos)
			return -1;
		std::string	 realname(line, pos + 1, i - pos - 1);
		names.push_back(realname);
		pos = line.find_first_not_of(" \t\n", i + 1);
		if (line.c_str()[pos] != ';')
			return -1;
		pos = line.find_first_not_of(" \t\n", pos + 1);
		if (line.c_str()[pos] != '#' && line.c_str()[pos] != '"' && pos != endl &&
			!(k == 1 && line.c_str()[pos] == '}'))
			return -1;
		if (line.c_str()[pos] == '#')
			pos = endl;
		k--;
	}
	while ((pos = line.find_first_not_of(" \t\n", pos)) == std::string::npos)
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
	}
	if (line.c_str()[pos] != '}')
		return -1;
	pos = line.find_first_not_of(" \t\n", pos + 1);
	if (line.c_str()[pos] != '#' && pos != endl)
		return -1;
	_server->hostname = names[0];
	_server->token = names[1];
	_server->info = names[2];
#if DEBUG_MODE
	std::cout << "server name: \"" << _server->hostname << '"';
	std::cout << " server token: \"" << _server->token << '"';
	std::cout << " server info: \"" << _server->info << '"' << '\n';
#endif
	return 0;
}

size_t	find_block(std::string line, size_t pos)
{
	if (!line.compare(pos, ft_strlen("listen"), "listen"))
		return LISTEN;
	if (!line.compare(pos, ft_strlen("admin"), "admin"))
		return ADMIN;
	if (!line.compare(pos, ft_strlen("link"), "link"))
		return LINK;
	if (!line.compare(pos, ft_strlen("me"), "me"))
		return ME;
	return std::string::npos;
}

void	server_init(IRCserv *_server, int ac, char **av)
{

	if (ac == 2)
	{

		std::string port(av[1]);

		if (port.find_first_not_of("0123456789") != std::string::npos ||
				port.length() < 1 || port.length() > 5)
			error_exit("Error: bad port number");
		_server->port = stoi(port);
		if (_server->port < 1 || _server->port > 65535)
			error_exit("Error: bad port number");
	}
	else if (ac == 3)
	{
		std::string port(av[1]);
		std::string pass(av[2]);

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
			t_strvect   temp;
			server_server   connect;

			temp = ft_splitstring(av[1], ":");
			if (temp.size() < 2 || temp.size() > 3)
				error_exit("Error: input parameters of server which you want to connect to is wrong!");
			connect.hostname = temp[0];
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
		std::string port(av[2]);
		std::string pass(av[3]);
		t_strvect   connect_to;
		server_server   temp;

		connect_to = ft_splitstring(av[1], ":");
		if (connect_to.size() < 2 || connect_to.size() > 3)
			error_exit("Error: input parameters of server which you want to connect to is wrong!");
		temp.type = TO;
		temp.hostname = connect_to[0];
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
	if (ac == 3 || ac == 4)
	{
		std::vector<t_link>::iterator	b = _server->link.begin();
		std::vector<t_link>::iterator	e = _server->link.end();
		int	flag;

		flag = 0;
		while (b != e)
		{
			if (b->hostname == _server->connect[0].hostname &&
					b->port == _server->connect[0].port &&
					b->pass == _server->connect[0].pass)
			{
				b->autoconnect = true;
				_server->connect[0].autoconnect = true;
				flag = 1;
			}
			b++;
		}
		if (!flag)
			error_exit("Error: server you are trying to connect bad configured");

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

void	parse(int ac, char **av, IRCserv *_server)
{
	std::fstream	config;
	std::string	line;
	size_t		line_number;
	size_t		i;
	t_blockmap	block;

	block[LISTEN] = block_listen;
	block[ADMIN] = block_admin;
	block[LINK] = block_link;
	block[ME] = block_me;
	config.open("./conf/ircserv.conf", std::ios::in);
	if (!config.is_open())
		error_exit("Error: can't open file \"./config/ircserv.conf\"");
	line_number = 0;
	while (!config.eof())
	{
		line_number++;
		getline(config, line);
		i = line.find_first_not_of(" \t\n");
		if (line.c_str()[i] == '#' || i == std::string::npos)
			continue ;
		i = find_block(line, i);
		if (i == std::string::npos)
			error_exit("Error: config error at line ", line, line_number);
		else
			if ((block[i](config, line, _server, line_number)) == -1)
			error_exit("Error: config error at line ", line, line_number);


	}
	server_init(_server, ac, av);
}
