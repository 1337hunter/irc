/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common_defines.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/25 18:46:30 by salec             #+#    #+#             */
/*   Updated: 2020/12/25 21:15:52 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_DEFINES_HPP
# define COMMON_DEFINES_HPP

# define CRLF			"\r\n"
# define FD_MAX			1024
# define FD_FREE		0
# define FD_CLIENT		1
# define FD_SERVER		2
# define FD_ME			3
# define FD_OPER		4
# define FD_SERVICE		5
# define FD_UNREGISTRED	6

# define BUF_SIZE		512
# define KILLTIME		60
# define BLOCKTIME		20

# define VERSION		"0210000000"
# define VERSIONSTRING	"IRCserv-0.2.1"

# ifdef DEBUG_MODE
#  define DEBUG_MODE	1
# else
#  define DEBUG_MODE	0
# endif

# define NPOS			std::string::npos

/*
# define REPLACE_LOCALTIME
# define NEED_GETTIMEOFDAY
# define NEED_MYTIMEFORMAT
# define STD_CPP98
*/

# ifndef STD_CPP98
#  define MAP			unordered_map
#  define TOSTRING		std::to_string
#  define STOI			std::stoi
#  define STOL			std::stol
#  define STOU			std::stou
#  define STOUL			std::stoul
# else
#  define MAP			map
#  define TOSTRING		ft_tostring
#  define STOI			ft_stoi
#  define STOL			ft_stol
#  define STOU			ft_stou
#  define STOUL			ft_stoul
# endif

typedef struct sockaddr		t_sockaddr;
typedef struct sockaddr_in	t_sockaddr_in;
typedef struct protoent		t_protoent;

#endif
