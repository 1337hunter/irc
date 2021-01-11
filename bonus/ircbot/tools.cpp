/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/09 18:57:17 by salec             #+#    #+#             */
/*   Updated: 2021/01/11 18:53:48 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <sstream>

typedef struct sockaddr_in	t_sockaddr_in;
typedef struct protoent		t_protoent;
typedef struct addrinfo		t_addrinfo;

void		error_exit(std::string const &s1)
{
	std::cerr << s1 << std::endl;
	exit(1);
}

std::string	getweather(std::string query)
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

int				ft_stoi(std::string const &str)
{
	std::istringstream	iss(str);
	int					val = 0;

	if (!(iss >> val))
		return (0);
	return (val);
}
