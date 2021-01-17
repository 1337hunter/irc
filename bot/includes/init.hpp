/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/14 17:20:43 by salec             #+#    #+#             */
/*   Updated: 2021/01/17 16:46:14 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef INIT_HPP
#define INIT_HPP

#include <string>

#define USAGESTRING std::string("Usage: ") + av[0] + \
	" [-h|--help] [-s|--service] [host] [port] [pass]\n" + \
	"Default host/port: localhost:6667"

int		initbot(t_strvect const &args, char **av, bool asService);

#endif
