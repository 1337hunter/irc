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

void	registerbot(int sock, std::string const &pass, bool asService)
{
	std::string	regmsg = "";
	std::string	nick = BOTNAME;

	if (pass != "")
		regmsg += "PASS " + pass + CRLF;
	if (!asService)
	{
		regmsg += "NICK " + nick + CRLF;
		regmsg += "USER " + nick + " 0 * :ircbot " + VERSION + CRLF;
	}
	else
		regmsg += "SERVICE " + nick + " * * 0 0 :ircbot v" + VERSION + CRLF;

	if (send(sock, regmsg.c_str(), regmsg.size(), 0) < 0)
		ioerror(sock, "send error on register");
}

int		initbot(t_strvect const &args, char **av, bool asService)
{
	std::string	host = "localhost";
	std::string	port = "6667";
	std::string	pass = "";

	if (args.size() >= 1)
		host = args[0];

	if (args.size() >= 2 &&
		((port = args[1]).find_first_not_of("0123456789") != std::string::npos ||
		ft_stoi(port) <= 0 || ft_stoi(port) > 65535))
		error_exit("Invalid port specified\n" + USAGESTRING);

	if (args.size() >= 3)
		pass = args[2];

	int	sock = initsock(host, port);
	std::cout << "Connected to " << host << ":" << port << std::endl;

	registerbot(sock, pass, asService);

	return (sock);
}
