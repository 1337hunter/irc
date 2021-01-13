/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 21:41:45 by salec             #+#    #+#             */
/*   Updated: 2021/01/13 23:37:19 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"

int		initbot(int ac, char **av)
{
	std::string	arg = "";

	if (ac > 1)
		arg = av[1];
	if (ac == 2 && (arg == "-h" || arg == "--help"))
		error_exit("Usage: " + std::string(av[0]) +
			" [-h|--help] <host> <port>\nDefault: localhost:6667");

	return (1);
}

int		main(int ac, char **av)
{
	int		sock = initbot(ac, av);
	ircbot	bot(sock);

	ReceiveMessage(bot);

	return (0);
}
