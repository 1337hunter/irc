/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/01/14 17:18:54 by salec             #+#    #+#             */
/*   Updated: 2021/01/14 19:50:22 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ircbot.hpp"
#include "init.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

typedef struct sockaddr_in	t_sockaddr_in;
typedef struct protoent		t_protoent;
typedef struct addrinfo		t_addrinfo;

int		initsock(std::string const &host, std::string const &port)
{
	int			sock = 0;

	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		error_exit("socket error");

	t_addrinfo	*addr;
	t_addrinfo	hints;
	hints.ai_flags = 0;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(host.c_str(), port.c_str(), &hints, &addr))
		ioerror(sock, "getaddrinfo error (" + host + ":" + port + ")");

	if (connect(sock, addr->ai_addr, addr->ai_addrlen) < 0)
	{
		freeaddrinfo(addr);
		ioerror(sock, "connect error (" + host + ":" + port + ")");
	}
	freeaddrinfo(addr);

	return (sock);
}

void	registerbot(int sock, std::string const &pass = "")
{
	std::string	regmsg = "";
	std::string	nick = BOTNAME;

	// not a SERVICE command for now
	if (pass != "")
		regmsg += "PASS " + pass + CRLF;
	regmsg += "NICK " + nick + CRLF;
	regmsg += "USER " + nick + " 0 * :" + nick + " " + VERSION + CRLF;

	if (send(sock, regmsg.c_str(), regmsg.size(), 0) < 0)
		ioerror(sock, "send error on register");
}

int		initbot(int ac, char **av)
{
	std::string	host = "localhost";
	std::string	port = "6667";
	std::string	pass = "";

	if (ac > 1)
		host = av[1];
	if (host == "-h" || host == "--help")
		error_exit(USAGESTRING);
	if (ac > 2 &&
		((port = av[2]).find_first_not_of("0123456789") != std::string::npos ||
		ft_stoi(port) <= 0 || ft_stoi(port) > 65535))
		error_exit("Invalid port specified\n" + USAGESTRING);
	if (ac > 3)
		pass = av[3];

	int	sock = initsock(host, port);
	std::cout << "Connected to " << host << ":" << port << std::endl;
	registerbot(sock, pass);
	return (sock);
}
