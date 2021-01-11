/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/09 16:37:43 by salec             #+#    #+#             */
/*   Updated: 2021/01/09 21:43:44 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"

int		main(int argc, char **argv)
{
	std::string	res;
	if (argc > 1)
		res = getweather(argv[1]);
	else
		res = getweather("Moscow");

	std::cout << "\ngot response:\n" << res << std::endl;

	t_weather	w;
	std::cout << "\nparsed response:" << std::endl;
	std::cout << "Country:\t" << w.country << std::endl;
	std::cout << "Location:\t" << w.location << std::endl;
	std::cout << "Weather:\t" << w.weather << std::endl;
	std::cout << "Current:\t" << w.temp_c << "C" << std::endl;
	std::cout << "Feelslike:\t" << w.feels_c << "C" << std::endl;
	std::cout << "Min temp:\t" << w.mintemp_c << "C" << std::endl;
	std::cout << "Max temp:\t" << w.maxtemp_c << "C" << std::endl;
	std::cout << "Pressure:\t" << w.press_mb << "mb" << std::endl;
	std::cout << "Pressure:\t" << w.press_mmhg << "mmhg" << std::endl;
	std::cout << "Wind speed:\t" << w.wind_speed << "m/s " << w.wind_deg << std::endl;
	std::cout << "Humidity:\t" << w.humidity << "%" << std::endl;

	return (0);
}
