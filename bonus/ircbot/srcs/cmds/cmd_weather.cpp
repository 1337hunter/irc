/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_weather.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 16:34:53 by salec             #+#    #+#             */
/*   Updated: 2021/01/13 17:15:50 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"
#include "cmds.hpp"
#include "tools.hpp"
#include "weather.hpp"

#define MAX_REQUEST_LEN	1024
#define BUF_SIZE		8192

typedef struct sockaddr_in	t_sockaddr_in;
typedef struct protoent		t_protoent;
typedef struct addrinfo		t_addrinfo;

void		weatherapiexception(jsonarray const &jsonweather)
{
	std::string	err = "server returned code ";
	err += jsonweather.at("cod").c_str();
	try { err += " (" + jsonweather.at("message") + ")"; }
	catch (std::exception const &e) { (void)e; }
	throw (std::invalid_argument(err.c_str()));
}

std::string	getweather(std::string query = "Moscow")
{
	int			sock = 0;
	std::string	hostname = "api.openweathermap.org";
	std::string	apikey = "081bf5c1866dd8f8eff826e30485f8fd";
	std::string	request =
		"GET /data/2.5/weather?APPID=" + apikey + "&q=" + query +
		" HTTP/1.1" + CRLF + "Host: " + hostname + CRLF + CRLF;

	std::cout << "request:" << std::endl << request << std::endl;

	/* Build the address. */
	t_addrinfo	*addr;
	t_addrinfo	hints;

	hints.ai_flags = 0;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		error_exit("socket error");
	if (getaddrinfo(hostname.c_str(), "80", &hints, &addr))
		error_exit("getaddrinfo error");

	/* Actually connect. */
	if (connect(sock, addr->ai_addr, addr->ai_addrlen) < 0)
		error_exit("connect error");
	freeaddrinfo(addr);

	/* Send HTTP request. */
	if (send(sock, request.c_str(), request.length(), 0) < 0)
		error_exit("send error");

	/* Read the response. */
	char	buf_read[BUF_SIZE];
	buf_read[0] = 0;
	ssize_t	r = recv(sock, buf_read, BUF_SIZE, 0);
	if (r < 0)
		error_exit("recv error");
	else
		buf_read[r] = 0;
	close(sock);

	std::string	res = buf_read;
	size_t		found = 0;
	if ((found = res.find("\r\n\r\n")) != std::string::npos)
	{
		std::cout << "got headers:\n" << res.substr(0, found) << std::endl;
		if (found + 4 < res.length())
			res = res.substr(found + 4);
	}

	return (res);
}

std::string	cmd_weather(t_strvect const &split)
{
	if (split.size() < 1)
		return ("");

	std::string	res;
	if (split.size() > 1)
		res = getweather(split[1]);
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
		w.press_mmhg = ft_stoi(jsontmp.at("pressure")) * 1000 / 1333;

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


	return ("");
}
