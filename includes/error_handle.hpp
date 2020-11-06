/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error_handle.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/18 17:48:42 by gbright           #+#    #+#             */
/*   Updated: 2020/11/06 21:18:22 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_HANDLE_HPP
# define ERROR_HANDLE_HPP

# include <iostream>
# define SELECT 0

void	error_exit(std::string s1);
void	error_exit(std::string s1, std::string line, size_t line_number);
void	error_exit(int code);
#endif
