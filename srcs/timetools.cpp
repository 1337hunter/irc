/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timetools.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/12/05 00:32:36 by salec             #+#    #+#             */
/*   Updated: 2020/12/05 02:21:50 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tools.hpp"
#include <sys/stat.h>		// fstat
#include <ctime>			// time_t, struct tm, strftime
#ifdef NEED_GETTIMEOFDAY
# include <sys/time.h>		// gettimeofday
#endif

// in case we actually use ft_localtime (REPLACE_LOCALTIME in common_defines)
#define TZ_OFFSET	3
#define	TZ_NAME		"MSK"

struct tm		*ft_localtime(time_t const *rawtime)
{
	struct tm	*res = new struct tm;

	res->tm_sec = 0;		// Seconds.			[0-60] (1 leap second)
	res->tm_min = 0;		// Minutes.			[0-59]
	res->tm_hour = 0;		// Hours.			[0-23]
	res->tm_mday = 0;		// Day.				[1-31]
	res->tm_mon = 0;		// Month.			[0-11]
	res->tm_year = 70;		// Year	- 1900.		[1970-...]
	res->tm_wday = 4;		// Day of week.		[0-6]
	res->tm_yday = 0;		// Days in year.	[0-365]
	res->tm_isdst = 0;		// DST.				[-1/0/1]
	#if defined(__USE_MISC) || defined(__DARWIN_STRUCT_STAT64)
		res->tm_hour = TZ_OFFSET;				// If supports timezones.
		res->tm_gmtoff = 3600L * TZ_OFFSET;		// Seconds east of UTC.
		res->tm_zone = TZ_NAME;					// Timezone abbreviation.
	#endif

	if (*rawtime < 0)
		return (res);
	time_t	timepart = (*rawtime) % 86400;
	time_t	datepart = (*rawtime) / 86400;

	res->tm_sec += timepart % 60;
	timepart /= 60;
	res->tm_min += timepart % 60;
	timepart /= 60;
	res->tm_hour += timepart;
	if (res->tm_hour > 23)
		datepart++;
	if (res->tm_hour < 0)
	{
		res->tm_hour += 23;
		datepart--;
	}
	res->tm_hour %= 24;

	res->tm_wday = (datepart + 4) % 7;
	int	daysinmonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	while (datepart > 0)
	{
		/*
			if (year is not divisible by 4) then (it is a common year)
			else if (year is not divisible by 100) then (it is a leap year)
			else if (year is not divisible by 400) then (it is a common year)
			else (it is a leap year)
		*/
		if (res->tm_year % 4 != 0)
			daysinmonth[1] = 28;
		else if (res->tm_year % 100 != 0)
			daysinmonth[1] = 29;
		else if (res->tm_year % 400 != 0)
			daysinmonth[1] = 28;
		else
			daysinmonth[1] = 29;

		int yday = 0;
		int month = -1;
		while (++month < 12 && daysinmonth[month] < datepart)
		{
			yday += daysinmonth[month];
			datepart -= daysinmonth[month];
		}
		if (month < 12 && datepart <= daysinmonth[month])
		{
			res->tm_mon = month;
			if (datepart < daysinmonth[month])
				res->tm_mday = datepart % daysinmonth[month];
			else
				res->tm_mday = datepart;
			res->tm_yday = yday + datepart - 1;
			break ;
		}
		res->tm_year++;
	}

	return (res);
}

time_t			ft_getcompiletime(void)
{
	int			fd = open("./ircserv", O_RDONLY);
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

#ifndef NEED_MYTIMEFORMAT
std::string		ft_timetostring(time_t rawtime)
{
	struct tm		*timeinfo;
	char			str[100];
	std::string		res = "";

	if (rawtime == (time_t)-1)
		return ("invalid rawtime");

	#ifndef REPLACE_LOCALTIME
		timeinfo = localtime(&rawtime);
	#else
		timeinfo = ft_localtime(&rawtime);
	#endif

	// asctime date format				"%a %b %e %T %Y"
	// RFC 2822-compliant date format	"%a, %d %b %Y %T %z"
	if (timeinfo != NULL)
		if (strftime(str, sizeof(str), "%a, %d %b %Y %T %z", timeinfo))
			res = str;

	#ifdef REPLACE_LOCALTIME
		delete (timeinfo);
	#endif

	return (res);
}
#else
std::string		ft_timetostring(time_t rawtime)
{
	struct tm		*timeinfo;
	std::string		res = "";

	if (rawtime == (time_t)-1)
		return ("invalid rawtime");

	#ifndef REPLACE_LOCALTIME
		timeinfo = localtime(&rawtime);
	#else
		timeinfo = ft_localtime(&rawtime);
	#endif

	if (timeinfo != NULL)
	{
		if (timeinfo->tm_mday < 10)
			res += "0";
		res += std::to_string(timeinfo->tm_mday) + "/";
		if (timeinfo->tm_mon + 1 < 10)
			res += "0";
		res += std::to_string(timeinfo->tm_mon + 1) + "/";
		res += std::to_string(timeinfo->tm_year + 1900) + " ";

		if (timeinfo->tm_hour < 10)
			res += "0";
		res += std::to_string(timeinfo->tm_hour) + ":";
		if (timeinfo->tm_min < 10)
			res += "0";
		res += std::to_string(timeinfo->tm_min) + ":";
		if (timeinfo->tm_sec < 10)
			res += "0";
		res += std::to_string(timeinfo->tm_sec);

		// tm_zone is GNU/BSD extension
		#if defined(__USE_MISC) || defined(__DARWIN_STRUCT_STAT64)
			res += + " ";
			res += (timeinfo->tm_zone);
		#endif
	}

	#ifdef REPLACE_LOCALTIME
		delete (timeinfo);
	#endif

	return (res);
}
#endif
