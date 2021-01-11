/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_handle.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/18 17:48:42 by gbright           #+#    #+#             */
/*   Updated: 2021/01/09 18:52:58 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_HANDLE_HPP
# define ERROR_HANDLE_HPP

# include <iostream>
# define SELECT 0

void	usage_exit(std::string const &exename);
void	error_exit(std::string const &s1);
void	error_exit(std::string const &s1, std::string const &line,
	size_t line_number);
void	error_exit(int code);

// put a std::string pointer to u if needed
int		SSLErrorCallback(const char *str, size_t len, void *u);

#endif
