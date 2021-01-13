/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/09 16:37:43 by salec             #+#    #+#             */
/*   Updated: 2021/01/13 17:43:42 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"
#include "jsonparser.hpp"

int		main(int argc, char **argv)
{
	std::string	res;
	t_strvect	split;
	split.push_back(":replyto");
	split.push_back("SQUERY");
	split.push_back("botname");
	split.push_back("!weather ");

	std::string	target = split[0].substr(1);
	if (argc > 1)
		split.back() += argv[1];
	else
		split.back() += "Moscow";

	if (split.size() < 4)
		return (1);
	split = ft_splitcmdbyspace(split[3]);
	res += cmd_weather(split);

	if (res.size() == 0)
		return (0);
	else
		res = res = "NOTICE " + target + " :" + res + CRLF;

	std::cout << std::endl << res;

	return (0);
}
