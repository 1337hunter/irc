/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 21:41:45 by salec             #+#    #+#             */
/*   Updated: 2021/01/13 22:26:07 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"

int		main(int argc, char **argv)
{
	ircbot		bot;

	t_strvect	split;
	split.push_back(":replyto");
	split.push_back("SQUERY");
	split.push_back("botname");

	if (argc > 1)
		split.push_back(argv[1]);
	else
		split.push_back(bot.prefix + std::string("help"));

	ProcessMessage(split, bot);

	return (0);
}
