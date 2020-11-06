/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/26 21:09:12 by salec             #+#    #+#             */
/*   Updated: 2020/11/06 19:40:01 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <string>
#include <vector>
#include "client.hpp"

typedef	std::vector<std::string>		t_strvect;
typedef std::vector<Client>::iterator	t_citer;

t_strvect	ft_splitstring(std::string msg, std::string const &delim);
t_citer		ft_findnick(t_citer const &begin, t_citer const &end,
				std::string const &nick);
t_citer		ft_findnick(t_citer const &begin, t_citer const &end,
				std::string const &nick);
std::string	ft_buildmsg(std::string const &srv, std::string const &msgcode,
	std::string const &target, std::string const &cmd, std::string const &msg);

#endif
