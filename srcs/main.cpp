/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 17:03:45 by salec             #+#    #+#             */
/*   Updated: 2020/11/20 11:45:29 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"
#include "error_handle.hpp"
#include "commands.hpp"
#include "tools.hpp"

int		main(int ac, char **av)
{
	IRCserv	serv;

	parse(ac, av, &serv);
	std::cout << "\n>>>> server goes brrrrrrrrr ..." << std::endl;
	/* init ssl lib and ctx */
	SSL_library_init();
	SSL_load_error_strings();
	InitSSLCTX(&serv);
	RunServer(&serv);
	return (0);
}
