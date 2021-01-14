/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 16:43:04 by salec             #+#    #+#             */
/*   Updated: 2021/01/14 19:00:52 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <iostream>
#include <string>
#include <vector>

typedef std::vector<std::string>	t_strvect;

void		error_exit(std::string const &s1);
void		ioerror(int sock, std::string const &msg);

int			ft_stoi(std::string const &str);
std::string	ft_tostring(int val);
std::string	ft_strtoupper(std::string const &str);
std::string	ft_strtolower(std::string const &str);
t_strvect	ft_splitcmdbyspace(std::string msg);
t_strvect	ft_splitstringbyany(std::string msg, std::string const &delim);

#endif
