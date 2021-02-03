#include "ircbot.hpp"
#include "init.hpp"
#include <algorithm>
#include <signal.h>

int		main(int ac, char **av)
{
	t_strvect	args(av + 1, av + ac);
	bool		asService = false;

	if (std::find(args.begin(), args.end(), "-h") != args.end() ||
		std::find(args.begin(), args.end(), "--help") != args.end())
		error_exit(USAGESTRING);

	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		error_exit("signal error");

	t_strvect::iterator	it = args.begin();
	while (it != args.end())
	{
		if (*it == "-s" || *it == "--service")
		{
			asService = true;
			it = args.erase(it);
		}
		else if ((*it).size() > 0 && (*it)[0] == '-')
			error_exit("ircbot: invalid option: " + *it + "\n" + USAGESTRING);
		else
			it++;
	}

	int		sock = initbot(args, av, asService);
	ircbot	bot(sock, asService);

	ReceiveMessages(bot);

	return (0);
}
