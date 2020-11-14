/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_handle.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/18 20:20:10 by salec             #+#    #+#             */
/*   Updated: 2020/11/14 17:50:34 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "error_handle.hpp"
#include "tools.hpp"

void	error_exit(std::string const &s1)
{
	std::cerr << s1 << std::endl;
	exit(1);
}

void	error_exit(std::string const &s1, std::string const &line, size_t n)
{

	std::cerr << s1 << n << std::endl;
	std::cout << line << std::endl;
	exit(1);
}

void	error_exit(int code)
{
	if (code == 0)
		std::cerr << "Error: select error" << std::endl;
	exit(1);
}

int		SSLErrorCallback(const char *str, size_t len, void *u)
{
	(void)u;
	if (len > 0)
	{
		t_strvect	errlines = ft_splitstring(str, "\n");
		for (size_t i = 0; i < errlines.size(); i++)
		{
			t_strvect	err = ft_splitstring(errlines[i], ":");
			std::cerr << "OpenSSL error";
			if (err.size() > 4)
				std::cerr << " in " << err[3] << ":" << err[4] << ".";
			if (err.size() > 5)
				std::cerr << " Reason: " << err[5];
			std::cerr << std::endl;
		}
	}
	return (1);
}
