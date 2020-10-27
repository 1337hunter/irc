/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/26 21:09:12 by salec             #+#    #+#             */
/*   Updated: 2020/10/27 16:02:10 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <string>
#include <vector>

typedef	std::vector<std::string>		t_strvect;

t_strvect		ft_splitstring(std::string msg, std::string const &delim);

#endif
