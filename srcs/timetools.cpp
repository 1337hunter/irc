#include "tools.hpp"
#include <sys/stat.h>
#include <ctime>
#ifdef NEED_GETTIMEOFDAY
# include <sys/time.h>
#endif

time_t			ft_getcompiletime(void)
{
	int			fd = open("./ircserv", O_RDONLY);
	time_t		rawtime = -1;

	if (fd > 0)
	{
		struct stat	stat;
		if (fstat(fd, &stat) == 0)
		{
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

#ifndef NEED_GETTIMEOFDAY
time_t			ft_getcurrenttime(void)
{
	return (time(NULL));
}
#else
time_t			ft_getcurrenttime(void)
{
	struct timeval	rawtime;

	if (gettimeofday(&rawtime, NULL) == 0)
		return (rawtime.tv_sec);
	return (-1);
}
#endif

std::string		ft_timetostring(time_t rawtime)
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

	return (res);
}
