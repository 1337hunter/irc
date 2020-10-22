/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/22 17:39:26 by salec             #+#    #+#             */
/*   Updated: 2020/10/22 18:18:50 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
#define USER_HPP

#include <iostream>
#include <string>

class User {
private:
	std::string		login;
	std::string		nick;
	std::string		real;
	// to be added: permissions idk
	User();
public:
	User(std::string const &login, std::string const &nick,
		std::string const &real);
	~User();
	User(User const &other);
	User &operator=(User const &other);

	// setters here maybe
};

#endif
