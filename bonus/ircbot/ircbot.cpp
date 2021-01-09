/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/09 16:37:43 by salec             #+#    #+#             */
/*   Updated: 2021/01/09 19:20:27 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"

int		main(int argc, char **argv)
{
	std::string	res;
	if (argc > 1)
		res = getweather(argv[1]);
	else
		res = getweather("Moscow");

	std::cout << "\ngot response:\n" << res << std::endl;

	return (0);
}
