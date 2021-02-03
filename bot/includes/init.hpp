#ifndef INIT_HPP
#define INIT_HPP

#include <string>

#define USAGESTRING std::string("Usage: ") + av[0] + \
	" [-h|--help] [-s|--service] [host] [port] [pass]\n" + \
	"Default host/port: localhost:6667"

int		initbot(t_strvect const &args, char **av, bool asService);

#endif
