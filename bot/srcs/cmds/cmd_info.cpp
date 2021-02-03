#include "cmds.hpp"
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

time_t		ft_getcompiletime(void)
{
	int			fd = open("./ircbot", O_RDONLY);
	time_t		rawtime = -1;

	if (fd > 0)
	{
		struct stat	stat;
		if (fstat(fd, &stat) == 0)
		{
			// stat struct is different on darwin
			#ifdef __DARWIN_STRUCT_STAT64
				rawtime = stat.st_mtimespec.tv_sec;
			#else
				rawtime = stat.st_mtim.tv_sec;
			#endif
		}
		close(fd);
	}
	return (rawtime);
}

std::string	gettimestr(time_t rawtime, std::string const &prestr)
{
	struct tm		*timeinfo;
	char			str[100];
	std::string		res = "";

	if (rawtime == (time_t)-1)
		return ("invalid rawtime");

	timeinfo = localtime(&rawtime);

	// asctime date format				"%a %b %e %T %Y"
	// RFC 2822-compliant date format	"%a, %d %b %Y %T %z"
	if (timeinfo != NULL)
		if (strftime(str, sizeof(str), "%a, %d %b %Y %T %z", timeinfo))
			res = str;

	if (res.size() > 0)
		return (prestr + res + "\n");
	return ("");
}

std::string	cmd_info(t_strvect const &split, ircbot const &bot)
{
	(void)split;
	std::string	res = "About " + bot.botname + " v" + bot.version + "\n";
	res += "Total commands: " +
		ft_tostring(static_cast<int>(bot.cmds.size())) + "\n";
	res += gettimestr(time(NULL), "Current time: ");
	res += gettimestr(ft_getcompiletime(), "Compile time: ");
	res += "Uses OpenWeatherMap API for weather info\n";
	return (res);
}
