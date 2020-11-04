/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_handle.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/18 20:20:10 by salec             #+#    #+#             */
/*   Updated: 2020/11/04 20:41:10 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error_handle.hpp"

void	error_exit(std::string s1)
{
	std::cerr << s1 << std::endl;
	exit(1);
}

void	error_exit(std::string s1, std::string line, size_t n)
{

	std::cerr << s1 << n << std::endl;
	std::cout << line << std::endl;
	exit(1);
}
