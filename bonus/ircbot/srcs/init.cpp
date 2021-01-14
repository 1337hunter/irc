/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/14 17:18:54 by salec             #+#    #+#             */
/*   Updated: 2021/01/14 17:22:10 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"
#include "init.hpp"

int		initbot(int ac, char **av)
{
	std::string	host = "localhost";
	std::string	port = "6667";
	std::string	pass = "";

	if (ac > 1)
		host = av[1];
	if (host == "-h" || host == "--help")
		error_exit(USAGESTRING);
	if (ac > 2 &&
		((port = av[2]).find_first_not_of("0123456789") != std::string::npos ||
		ft_stoi(port) <= 0 || ft_stoi(port) > 65535))
		error_exit("Invalid port specified\n" + USAGESTRING);
	if (ac > 3)
		pass = av[3];


	return (1);
}
