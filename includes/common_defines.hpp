/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   common_defines.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/25 18:46:30 by salec             #+#    #+#             */
/*   Updated: 2020/11/10 13:23:57 by gbright          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMON_DEFINES_HPP
# define COMMON_DEFINES_HPP

# define FD_MAX		1024
# define FD_FREE	0
# define FD_CLIENT	1
# define FD_SERVER	2
# define FD_ME		3
# define FD_OPER	4

# define BUF_SIZE	512

# define DEBUG_MODE	1

// # define KONVERSATION

# ifndef KONVERSATION
#  define CRLF		"\r\n"
# else
#  define CRLF		"\n"	// konversation sends only \n for some reason
# endif

typedef struct sockaddr		t_sockaddr;
typedef struct sockaddr_in	t_sockaddr_in;
typedef struct protoent		t_protoent;

#endif
