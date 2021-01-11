/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ircbot.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/09 16:37:43 by salec             #+#    #+#             */
/*   Updated: 2021/01/11 19:14:28 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"
#include "jsonparser.hpp"

void	weatherapiexception(jsonarray const &jsonweather)
{
	std::string	err = "server returned code ";
	err += jsonweather.at("cod").c_str();
	try { err += " (" + jsonweather.at("message") + ")"; }
	catch (std::exception const &e) { (void)e; }
	throw (std::invalid_argument(err.c_str()));
}

int		main(int argc, char **argv)
{
	std::string	res;
	if (argc > 1)
		res = getweather(argv[1]);
	else
		res = getweather("Moscow");

	std::cout << "\ngot response:\n" << res << "\n";

	t_weather	w;
	try
	{
		jsonarray	jsonweather = jsonparse(res);
		if (jsonweather.at("cod") != "200")
			weatherapiexception(jsonweather);

		// getting location info
		w.location = jsonweather.at("name");
		jsonarray	jsontmp = jsonparse(jsonweather.at("sys"));
		w.country = jsontmp.at("country");

		// getting main info
		jsontmp = jsonparse(jsonweather.at("main"));
		w.temp_c = ft_stoi(jsontmp.at("temp")) - 273;
		w.feels_c = ft_stoi(jsontmp.at("feels_like")) - 273;
		w.mintemp_c = ft_stoi(jsontmp.at("temp_min")) - 273;
		w.maxtemp_c = ft_stoi(jsontmp.at("temp_max")) - 273;
		w.humidity = ft_stoi(jsontmp.at("humidity"));
		w.press_mb = ft_stoi(jsontmp.at("pressure"));
		w.press_mmhg = w.press_mb * 1000 / 1333;

		// getting wind info
		jsontmp = jsonparse(jsonweather.at("wind"));
		w.wind_speed = ft_stoi(jsontmp.at("speed"));
		w.wind_deg = ft_stoi(jsontmp.at("deg"));

		// getting weather info
		std::string	tmp = jsonweather.at("weather");
		if (tmp[0] == '[')
			tmp = tmp.substr(1, tmp.size() - 2);
		jsontmp = jsonparse(tmp);
		w.type = jsontmp.at("main");
		w.descr = jsontmp.at("description");

		std::cout << "\nparsed response:" << std::endl;
		std::cout << "Country:\t" << w.country << std::endl;
		std::cout << "Location:\t" << w.location << std::endl;
		std::cout << "Weather:\t" << w.type << std::endl;
		std::cout << "Description:\t" << w.descr << std::endl;
		std::cout << "Current:\t" << w.temp_c << "\u2103" << std::endl;
		std::cout << "Feels like:\t" << w.feels_c << "\u2103" << std::endl;
		std::cout << "Min temp:\t" << w.mintemp_c << "\u2103" << std::endl;
		std::cout << "Max temp:\t" << w.maxtemp_c << "\u2103" << std::endl;
		std::cout << "Pressure:\t" << w.press_mb << "mb" << std::endl;
		std::cout << "Pressure (ru):\t" << w.press_mmhg << "mmhg" << std::endl;
		std::cout << "Wind speed:\t" << w.wind_speed << "m/s " << w.wind_deg << "\u00B0" << std::endl;
		std::cout << "Humidity:\t" << w.humidity << "%" << std::endl;
	}
	catch (std::invalid_argument const &e)
	{
		std::cerr << std::endl << e.what() << std::endl;
	}
	catch (std::out_of_range const &e)
	{
		std::cerr << std::endl << "server api didn't return the code in json" << std::endl;
	}


	return (0);
}
