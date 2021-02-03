#ifndef WEATHER_HPP
#define WEATHER_HPP

#include "jsonparser.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef WEATHERAPIKEY
#define WEATHERAPIKEY	"insertapikey"
#endif

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
