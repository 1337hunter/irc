/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common_defines.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/25 18:46:30 by salec             #+#    #+#             */
/*   Updated: 2020/12/05 01:26:48 by salec            ###   ########.fr       */
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

# define BUF_SIZE		512

# define VERSIONSTRING	"IRCserv-0.2.1"

# define DEBUG_MODE		1

/*
# define REPLACE_LOCALTIME
# define NEED_GETTIMEOFDAY
# define NEED_MYTIMEFORMAT
*/

typedef struct sockaddr		t_sockaddr;
typedef struct sockaddr_in	t_sockaddr_in;
typedef struct protoent		t_protoent;

#endif
