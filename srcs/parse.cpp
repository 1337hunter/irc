/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 15:56:53 by gbright           #+#    #+#             */
/*   Updated: 2020/11/04 23:53:06 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error_handle.hpp"
#include "ircserv.hpp"
#include <string>
#include <iostream>
#include <fstream>

#define LISTEN 0
#define LINCK 1
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

	pos = 0;
	while (line.c_str()[pos] && line.c_str()[pos] < ' ')
		pos++;
	if ((line.compare(pos, ft_strlen("listen"), "listen")) != 0)
		return -1;
	if (line.find_first_not_of(" \t\n{", pos + ft_strlen("listen")) != std::string::npos)
		return -1;
	while (line.find("}") == std::string::npos)
	{
		getline(config, line);
		line_number++;
		if (config.eof())
			return -1;
		pos = 0;
		while (line.c_str()[pos] && line.c_str()[pos] < ' ')
			pos++;
		if (line.compare(pos, 1, "{") == 0)
			continue ;
		else if (!line.compare(pos, 2, "ip"))
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
			if (pos != std::string::npos && line.c_str()[pos] != '#')
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
			if (pos != std::string::npos && line.c_str()[pos] != '#')
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
			while (line.c_str()[pos] != '}')
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
				pos = line.find_first_not_of(" \n\t", pos);
			}
		}

	}
	_server->listen.push_back(temp);
#if DEBUG_MODE
	std::cout << "listen: ip " << _server->listen[0].ip << " port " << _server->listen[0].port << ' ';
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
	if (_server->admin.set)
		return -1;
	(void)config;
	(void)line;
	_server = 0;
	line_number++;
	return 0;
}


size_t	find_block(std::string line)
{
	size_t	pos;

	pos = line.find_first_not_of(" \n\t");
	if (pos == std::string::npos)
		return pos;
	if (!(line.compare(pos, ft_strlen("listen"), "listen")))
		return LISTEN;
	if (!(line.compare(pos, ft_strlen("admin"), "admin")))
		return ADMIN;
	return std::string::npos;
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
	config.open("./conf/ircserv.conf", std::ios::in);
	if (!config.is_open())
		error_exit("Error: can't open file \"./config/ircserv.conf\"");
	line_number = 0;
	while (!config.eof())
	{
		line_number++;
		getline(config, line);
		i = 0;
		while (line.c_str()[i] < ' ' && line.c_str() != 0) //escape comments
			i++;
		if (line.c_str()[i] == '#')
			continue ;
		i = find_block(line);
		if (i != std::string::npos)
			if ((block[i](config, line, _server, line_number)) == -1)
			error_exit("Error: config error at line ", line, line_number);


	}
	av = 0;
	ac = 0;
	line_number = 0;
	_server = 0;
}
