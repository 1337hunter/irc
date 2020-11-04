/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gbright <gbright@student.21-school.ru>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/11/04 15:56:53 by gbright           #+#    #+#             */
/*   Updated: 2020/11/04 16:37:48 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "error_handle.hpp"
#include <fcntl.h>

void	parse(int ac, char **av)
{
	int	config;

	if ((config = open("./conf/ircserv.conf", O_RDONLY) < 0))
		error_exit("Error: can not open file: ircserv.conf");
	av = 0;
	ac = 0;
}
