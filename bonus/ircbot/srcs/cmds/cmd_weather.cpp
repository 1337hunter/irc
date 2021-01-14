/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_weather.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/13 16:34:53 by salec             #+#    #+#             */
/*   Updated: 2021/01/14 21:15:50 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"
#include "cmds.hpp"
#include "weather.hpp"

#define MAX_REQUEST_LEN	1024
#define BUF_SIZE		8192

typedef struct sockaddr_in	t_sockaddr_in;
typedef struct protoent		t_protoent;
typedef struct addrinfo		t_addrinfo;

void		weatherapiexception(jsonarray const &jsonweather)
{
	std::string	err = "api returned code ";
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
//	std::cout << "request:" << std::endl << request << std::endl;

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		return("api connection error (socket)");

	t_addrinfo	*addr;
	t_addrinfo	hints;
	hints.ai_flags = 0;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(hostname.c_str(), "80", &hints, &addr))
	{
		close(sock);
		return("api connection error (getaddrinfo)");
	}

	if (connect(sock, addr->ai_addr, addr->ai_addrlen) < 0)
	{
		freeaddrinfo(addr);
		close(sock);
		return("api connection error (connect)");
	}
	freeaddrinfo(addr);

	if (send(sock, request.c_str(), request.length(), 0) < 0)
	{
		close(sock);
		return("api connection error (send)");
	}

	char	buf_read[BUF_SIZE];
	buf_read[0] = 0;
	ssize_t	r = recv(sock, buf_read, BUF_SIZE, 0);
	if (r < 0)
	{
		close(sock);
		return("api connection error (recv)");
	}
	close(sock);
	buf_read[r] = 0;

	std::string	res = buf_read;
	size_t		found = 0;
	if ((found = res.find("\r\n\r\n")) != std::string::npos)
	{
//		std::cout << "got headers:\n" << res.substr(0, found) << std::endl;
		if (found + 4 < res.length())
			res = res.substr(found + 4);
	}

	return (res);
}

std::string	cmd_weather(t_strvect const &split, ircbot const &bot)
{
	(void)bot;
	if (split.size() < 1)
		return ("");

	std::string	res;
	if (split.size() > 1)
	{
		std::string	query;
		for (size_t i = 1; i < split.size(); i++)
			query += split[i] + " ";
		res = getweather(query);
	}
	else
		res = getweather("Moscow");

//	std::cout << "response:\n" << res << "\n";

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

		res = "";
/*
		std::cout << "parsed response:" << std::endl;
		std::cout << "Country:\t" << w.country << std::endl;
		std::cout << "Location:\t" << w.location << std::endl;
		std::cout << "Weather:\t" << w.type << std::endl;
		std::cout << "Description:\t" << w.descr << std::endl;
		std::cout << "Current:\t" << w.temp_c << "\u2103" << std::endl;
		std::cout << "Feels like:\t" << w.feels_c << "\u2103" << std::endl;
		std::cout << "Min temp:\t" << w.mintemp_c << "\u2103" << std::endl;
		std::cout << "Max temp:\t" << w.maxtemp_c << "\u2103" << std::endl;
		std::cout << "Pressure:\t" << w.press_mmhg << "mmhg" << std::endl;
		std::cout << "Wind speed:\t" << w.wind_speed << "m/s " << w.wind_deg << "\u00B0" << std::endl;
		std::cout << "Humidity:\t" << w.humidity << "%" << std::endl;
*/
		res += "Weather in " + w.location + "," + w.country + "\n" +
			w.type + " (" + w.descr + ")\n" +
			"Temp: " + ft_tostring(w.temp_c) + "\u2103 " + // "C " +
			"(feels like " + ft_tostring(w.feels_c) + "\u2103)\n" + // "C)\n" +
			"Min: " + ft_tostring(w.mintemp_c) + "\u2103 | " + // "C " +
			"Max: " + ft_tostring(w.maxtemp_c) + "\u2103\n" + // "C\n" +
			"Wind: " + ft_tostring(w.wind_speed) + "m/s " +
			"direction " + ft_tostring(w.wind_deg) + "\u00B0" + "\n" +
			"Pressure: " + ft_tostring(w.press_mmhg) + "mmhg\n" +
			"Humidity: " + ft_tostring(w.humidity) + "%";
	}
	catch (std::invalid_argument const &e)
	{
		res = e.what();
//		std::cerr << std::endl << e.what() << std::endl;
	}
	catch (std::out_of_range const &e)
	{
		res = "Weather api json error";
//		std::cerr << std::endl << "server api didn't return the code in json" << std::endl;
	}
	catch (std::exception const &e)
	{
		res = "Undefined api error";
//		std::cerr << std::endl << "server api didn't return the code in json" << std::endl;
	}

	return (res);
}
