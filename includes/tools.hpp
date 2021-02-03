#ifndef TOOLS_HPP
# define TOOLS_HPP

# include <string>
# include <vector>
# include "client.hpp"
# include <list>
# include "ircserv.hpp"

typedef	std::vector<std::string>		t_strvect;
typedef std::list<Client>::iterator		t_citer;

/****************************************************/
/*	additional tools helping the server (tools.cpp)	*/
/****************************************************/
t_citer		ft_findclientfd(t_citer const &begin, t_citer const &end, int fd);
t_citer		ft_findnick(t_citer const &begin, t_citer const &end,
				std::string const &nick);
t_server    *find_server_by_fd(int fd, IRCserv *serv);
Client  	*find_client_by_nick(std::string const &nick, IRCserv *serv);
Client		*find_client_by_fd(int fd, IRCserv *serv);
Channel		*find_channel_by_name(const std::string &name, IRCserv *serv);
t_server    *find_server_by_mask(std::string const &mask, IRCserv *serv);
t_server    *find_server_by_name(std::string const &name, IRCserv *serv);
Client      *find_client_by_user_or_nick_and_host(std::string const &str, IRCserv *serv);
Client      *find_client_by_info(std::string const &info, IRCserv *serv);
t_server    *find_server_by_token(std::string const &token, IRCserv *serv);
t_service	*find_service_by_name(std::string const &name, IRCserv *serv);
t_service	*find_service_by_fd(int fd, IRCserv *serv);
std::string	get_servername_by_mask(std::string const &mask, IRCserv *serv);
std::string get_servername_by_token(std::string const &token, IRCserv *serv);
std::string get_hopcount_by_token(std::string const &token, IRCserv *serv);

std::string	ft_buildmsg(std::string const &from, std::string const &msgcode,
	std::string const &to, std::string const &cmd, std::string const &msg);
void		addtonickhistory(IRCserv *serv, t_citer const client);
int			nick_forward(IRCserv *serv, Client *client);

bool    remove_channel(Channel *channel, IRCserv *serv);
bool    remove_client_by_ptr(Client *ptr, IRCserv *serv);
bool    remove_client_by_nick(std::string const &nick, IRCserv *serv);
void    remove_server_by_name(std::string const &servername, IRCserv *serv);

void    self_cmd_squit(int fd, t_fd &fdref, IRCserv *serv);
void    self_cmd_quit(int fd, t_fd &fdref, IRCserv *serv, std::string const &reason);
void    self_service_quit(int fd, t_fd &fdref, IRCserv *serv);

/****************************************************/
/*	string manipulation functions (stringtools.cpp)	*/
/****************************************************/
t_strvect	ft_splitstring(std::string msg, std::string const &delim);
t_strvect	ft_splitstring(std::string str, char delim);
t_strvect	ft_splitstringbyany(std::string msg, std::string const &delim);
/**
 *	ft_splitcmdbyspace
 *	splits until the second occurance of ':' symbol
 *	(special for irc msg format)
 */
t_strvect	ft_splitcmdbyspace(std::string msg);

std::string	strvect_to_string(const t_strvect &split, char delim = ' ',
	size_t pos = 0, size_t len = std::string::npos);
bool		match(const char *s1, const char *s2);
bool		match(std::string const &s1, std::string const &s2);
std::string	ft_strtoupper(std::string const &str);
std::string	ft_strtolower(std::string const &str);
std::string	get_nick_from_info(std::string const &info);
std::string get_mask_reply(Channel *channel, Client *client, std::string mode, IRCserv *serv);
bool    	is_valid_mask(std::string mask);
bool    	is_valid_serv_host_mask(std::string mask);
//	ft_tostring
std::string	ft_tostring(int val);
std::string	ft_tostring(long val);
std::string	ft_tostring(uint val);
std::string	ft_tostring(ulong val);
//	ft_sto*
int			ft_stoi(std::string const &str);
long		ft_stol(std::string const &str);
uint		ft_stou(std::string const &str);
ulong		ft_stoul(std::string const &str);

/****************************************************/
/*		time related functions (timetools.cpp)		*/
/****************************************************/
time_t		ft_getcompiletime(void);
time_t		ft_getcurrenttime(void);
std::string	ft_timetostring(time_t rawtime);

#endif
