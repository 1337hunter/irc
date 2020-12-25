/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 17:03:45 by salec             #+#    #+#             */
/*   Updated: 2020/12/25 16:09:09 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "error_handle.hpp"
#include "commands.hpp"
#include "tools.hpp"

bool	g_server_die;
bool	g_rehash;

int		main(int ac, char **av)
{
	IRCserv	*serv = 0;
	time_t	compiled;
	time_t	started;


	SSL_library_init();
	SSL_load_error_strings();
	if (DEBUG_MODE)
		std::cout << std::endl;
	std::cout << ">>>> " << VERSIONSTRING << " is starting ..." << std::endl;
	std::cout << "Version:\t" << VERSION << std::endl;
	compiled = ft_getcompiletime();
	started = ft_getcurrenttime();
	std::cout << "Compiled:\t" << ft_timetostring(compiled) << std::endl;
	std::cout << "Started:\t" << ft_timetostring(started) << std::endl;

	g_server_die = false;
	while (true)
	{
		g_rehash = false;
		serv = new IRCserv;
		InitSSLCTX(serv);
		initcommands(serv);
		parse(ac, av, serv);
		serv->dtstarted = started;
		serv->dtcompiled = compiled;
		RunServer(serv);
	}
	return (0);
}
