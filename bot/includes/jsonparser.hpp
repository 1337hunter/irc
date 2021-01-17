/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   jsonparser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/11 17:33:47 by salec             #+#    #+#             */
/*   Updated: 2021/01/14 17:34:12 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef JSONPARSER_HPP
#define JSONPARSER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <exception>

typedef std::pair<std::string, std::string>	keyval;
typedef std::map<std::string, std::string>	jsonarray;

jsonarray		jsonparse(std::string const &input);

#endif
