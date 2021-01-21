/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common_defines.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/25 18:46:30 by salec             #+#    #+#             */
/*   Updated: 2021/01/22 02:12:50 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_DEFINES_HPP
# define COMMON_DEFINES_HPP

# define CRLF				"\r\n"
# define FD_FREE			0
# define FD_CLIENT			1
# define FD_SERVER			2
# define FD_ME				3
# define FD_OPER			4
# define FD_SERVICE			5
# define FD_UNREGISTRED		6

# ifndef FD_MAX
#  define FD_MAX			1024
# endif

# if FD_MAX < 10
#  undef FD_MAX
#  define FD_MAX			1024
# endif

# if FD_MAX > 10000
#  undef FD_MAX
#  define FD_MAX			1024
# endif

# ifndef BUF_SIZE
#  define BUF_SIZE			512
# endif

# if BUF_SIZE < 10
#  undef BUF_SIZE
#  define BUF_SIZE			512
# endif

# if BUF_SIZE > 10000
#  undef BUF_SIZE
#  define BUF_SIZE			512
# endif

# ifndef WHOWAS_MAX
#  define WHOWAS_MAX		2000
# endif

# if WHOWAS_MAX < 10
#  undef WHOWAS_MAX
#  define WHOWAS_MAX		2000
# endif

# if WHOWAS_MAX > 10000
#  undef WHOWAS_MAX
#  define WHOWAS_MAX		2000
# endif

# ifndef PING_TIMEOUT
#  define PING_TIMEOUT		60
# endif

# if PING_TIMEOUT < 10
#  undef PING_TIMEOUT
#  define PING_TIMEOUT		60
# endif

# if PING_TIMEOUT > 100000
#  undef PING_TIMEOUT
#  define PING_TIMEOUT		60
# endif

# ifndef PING_FREQUENCY
#  define PING_FREQUENCY	60
# endif

# if PING_FREQUENCY < 10
#  undef PING_FREQUENCY
#  define PING_FREQUENCY	60
# endif

# if PING_FREQUENCY > 100000
#  undef PING_FREQUENCY
#  define PING_FREQUENCY	60
# endif

# define KILLTIME			60
# define BLOCKTIME			20

# define VERSION			"0210000000"
# define VERSIONSTRING		"IRCserv-0.2.1"

# ifndef DEBUG_MODE
#  define DEBUG_MODE		0
# endif

# define NPOS				std::string::npos

/*
# define REPLACE_LOCALTIME
# define NEED_GETTIMEOFDAY
# define NEED_MYTIMEFORMAT
# define STD_CPP98
*/

# ifndef STD_CPP98
#  define MAP				unordered_map
#  define TOSTRING			std::to_string
#  define STOI				std::stoi
#  define STOL				std::stol
#  define STOU				std::stou
#  define STOUL				std::stoul
# else
#  define MAP				map
#  define TOSTRING			ft_tostring
#  define STOI				ft_stoi
#  define STOL				ft_stol
#  define STOU				ft_stou
#  define STOUL				ft_stoul
# endif

typedef struct sockaddr		t_sockaddr;
typedef struct sockaddr_in	t_sockaddr_in;
typedef struct protoent		t_protoent;

typedef unsigned long		ulong;	// not defined on Darwin 18.7

#endif
