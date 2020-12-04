/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 17:03:45 by salec             #+#    #+#             */
/*   Updated: 2020/12/04 18:39:12 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "error_handle.hpp"
#include "commands.hpp"
#include "tools.hpp"

int		main(int ac, char **av)
{
	IRCserv	*serv = new IRCserv;

	parse(ac, av, serv);

	/* init ssl lib and ctx */
	SSL_library_init();
	SSL_load_error_strings();
	InitSSLCTX(serv);

	if (DEBUG_MODE)
		std::cout << std::endl;
	std::cout << ">>>> server is starting ..." << std::endl;
	serv->dtcompiled = ft_getcompiletime();
	serv->dtstarted = ft_getcurrenttime();
	std::cout << "Compiled:\t" << ft_timetostring(serv->dtcompiled) << std::endl;
	std::cout << "Started:\t" << ft_timetostring(serv->dtstarted) << std::endl;

	RunServer(serv);
	return (0);
}
