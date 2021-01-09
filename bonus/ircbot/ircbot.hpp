/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/09 18:57:38 by salec             #+#    #+#             */
/*   Updated: 2021/01/09 21:33:50 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRCBOT_HPP
#define IRCBOT_HPP

#include <iostream>
#include <string>

#define MAX_REQUEST_LEN	1024
#define BUF_SIZE		8192
#define CRLF 			"\r\n"

typedef struct	s_weather
{
	std::string	location;
	std::string	country;
	std::string	weather;
	int			temp_c;
	int			feels_c;
	int			mintemp_c;
	int			maxtemp_c;
	int			press_mb;
	int			press_mmhg;
	int			wind_speed;
	int			wind_deg;
	int			humidity;
}				t_weather;

void			error_exit(std::string const &s1);

std::string		getweather(std::string query = "Moscow");

#endif
