/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_help.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 19:07:03 by salec             #+#    #+#             */
/*   Updated: 2021/01/13 21:48:25 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cmds.hpp"

std::string	cmd_help(t_strvect const &split, ircbot const &bot)
{
	(void)split;
	std::string	res = "";

	for (t_cmdmap::const_iterator it = bot.cmds.begin();
		it != bot.cmds.end(); it++)
		res += bot.prefix + it->first + " - " + it->second.getinfo() + '\n';

	return (res);
}
