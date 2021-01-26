#include "ircserv.hpp"
#include "error_handle.hpp"
#include "commands.hpp"
#include "tools.hpp"
#include <signal.h>

bool	g_server_die;
bool	g_rehash;

int		main(int ac, char **av)
{
	IRCserv	*serv = NULL;
	if (ac < 2 || ac > 4)
		usage_exit(av[0]);

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		error_exit("signal error");

	SSL_library_init();
	SSL_load_error_strings();
	if (DEBUG_MODE)
		std::cout << std::endl;
	std::cout << ">>>> " << VERSIONSTRING << " is starting ..." << std::endl;
	std::cout << "Version:\t" << VERSION << std::endl;
	time_t	compiled = ft_getcompiletime();
	time_t	started = ft_getcurrenttime();
	std::cout << "Compiled:\t" << ft_timetostring(compiled) << std::endl;
	std::cout << "Started:\t" << ft_timetostring(started) << std::endl;
#if DEBUG_MODE
	std::cout << "PING_TIMEOUT=" << PING_TIMEOUT << "\n";
	std::cout << "PING_FREQUENCY=" << PING_FREQUENCY << "\n";
#endif

	g_server_die = false;
	while (true)
	{
		g_rehash = false;
		serv = new IRCserv;
		InitSSLCTX(serv);
		initcommands(serv);
		parse(ac, av, serv);
		defineservermodes(serv);
		serv->dtstarted = started;
		serv->dtcompiled = compiled;
		RunServer(serv);
	}
	return (0);
}
