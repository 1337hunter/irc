/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 21:41:45 by salec             #+#    #+#             */
/*   Updated: 2021/01/14 18:47:30 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"
#include "init.hpp"

int		main(int ac, char **av)
{
	int		sock = initbot(ac, av);
	ircbot	bot(sock);

	ReceiveMessages(bot);

	return (0);
}
