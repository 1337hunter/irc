/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/26 21:09:12 by salec             #+#    #+#             */
/*   Updated: 2020/11/23 12:02:52 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <string>
#include <vector>
#include "client.hpp"

typedef	std::vector<std::string>		t_strvect;
typedef std::vector<Client>::iterator	t_citer;

t_citer		ft_findclientfd(t_citer const &begin, t_citer const &end, int fd);
t_citer		ft_findnick(t_citer const &begin, t_citer const &end,
				std::string const &nick);
t_strvect	ft_splitstring(std::string msg, std::string const &delim);
std::string	strvect_to_string(const t_strvect &split, char delimiter = ' ', size_t position = 0, size_t len = std::string::npos);
std::string	ft_buildmsg(std::string const &srv, std::string const &msgcode,
	std::string const &target, std::string const &cmd, std::string const &msg);
int	match(const char *s1, const char *s2);

#endif
