/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 17:03:45 by salec             #+#    #+#             */
/*   Updated: 2020/10/22 17:09:21 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircserv.hpp"

int		main(int ac, char **av)
{
	IRCserv	*irc = NULL;

	if (ac == 2)
		irc = new IRCserv(av[1]);
	else if (ac == 3)
		irc = new IRCserv(av[1], av[2]);
	else if (ac == 4)
		irc = new IRCserv(av[1], av[2], av[3]);
	else
		std::cerr << "Usage: " << av[0] << " [host:port_network:" <<
			"password_network] <port> <password>" << std::endl;
	delete (irc);
	return (0);
}
