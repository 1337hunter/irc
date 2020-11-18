/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_handle.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/18 20:20:10 by salec             #+#    #+#             */
/*   Updated: 2020/11/18 19:09:56 by salec            ###   ########.fr       */
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

// put a std::string pointer to u
int		SSLErrorCallback(const char *str, size_t len, void *u)
{
	std::string		*outptr = NULL;

	if (u)
		outptr = reinterpret_cast<std::string*>(u);
	if (len > 0)
	{
		t_strvect	errlines = ft_splitstring(str, "\n");
		for (size_t i = 0; i < errlines.size(); i++)
		{
			t_strvect	err = ft_splitstring(errlines[i], ":");
			std::cerr << "OpenSSL error";
			if (err.size() > 5)
				std::cerr << " in " << err[3] << ":" << err[4] <<
					". Reason: " + err[5];
			std::cerr << std::endl;
			if (outptr)
			{
				(*outptr) += "OpenSSL error";
				if (err.size() > 5)
					(*outptr) += " in " + err[3] + ":" + err[4] +
						". Reason: " + err[5] + ";";
			}
		}
	}
	return (1);
}
