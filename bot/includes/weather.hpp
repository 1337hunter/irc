/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   weather.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 16:56:31 by salec             #+#    #+#             */
/*   Updated: 2021/01/17 18:19:41 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEATHER_HPP
#define WEATHER_HPP

#include "jsonparser.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define WEATHERAPIKEY	"081bf5c1866dd8f8eff826e30485f8fd"

typedef struct	s_weather
{
	std::string	location;
	std::string	country;
	std::string	type;
	std::string	descr;
	int			temp_c;
	int			feels_c;
	int			mintemp_c;
	int			maxtemp_c;
	int			press_mmhg;
	int			wind_speed;
	int			wind_deg;
	int			humidity;
}				t_weather;

#endif
