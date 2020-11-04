/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 15:56:53 by gbright           #+#    #+#             */
/*   Updated: 2020/11/04 19:38:50 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error_handle.hpp"
#include "ircserv.hpp"
#include <string>
#include <iostream>
#include <fstream>

#define LISTEN 0

typedef	void (*t_block)(std::fstream &config, std::string &line, IRCserv *_server);
typedef std::map<int, t_block> t_blockmap;

void	block_listen(std::fstream &config, std::string &line, IRCserv *_server)
{
	(void)config;
	line = "0";
	_server = 0;
}

size_t	find_block(std::string line)
{
	size_t	pos;

	if ((pos = line.find("listen")) != std::string::npos)
		return LISTEN;
	return std::string::npos;
}

void	parse(int ac, char **av, IRCserv *_server)
{
	std::fstream	config;
	std::string	line;
	size_t		line_number;
	size_t		pos;
	size_t		i;
	t_blockmap	block;

	block[LISTEN] = block_listen;
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
		if ((pos = line.find('{')) != std::string::npos) //find first block
			i = find_block(line);
		if (i != std::string::npos)
			block[i](config, line, _server);
	}
	av = 0;
	ac = 0;
	line_number = 0;
	_server = 0;
}
