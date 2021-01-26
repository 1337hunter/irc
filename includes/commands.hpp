#ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include "ircserv.hpp"
# include <string>

struct		IRCserv;

void	initcommands(IRCserv *serv);

void	cmd_nick(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_user(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_ping(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_pong(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_quit(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_oper(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_server(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_pass(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_squit(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_connect(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_error(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_admin(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_motd(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_userhost(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_version(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_info(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_time(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_join(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_privmsg(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_invite(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_names(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_who(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_whois(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_whowas(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_part(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_mode(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_notice(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_away(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_kill(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_stats(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_links(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_njoin(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_lusers(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_ison(int fd, const t_strvect &split, IRCserv *serv);
void	cmd_users(int fd, const t_strvect &split, IRCserv *serv);	// disabled
void    cmd_topic(int fd, t_strvect const &split, IRCserv *serv);
void	cmd_kick(int fd, t_strvect const &split, IRCserv *serv);
void	cmd_trace(int fd, t_strvect const &split, IRCserv *serv);
void	cmd_die(int fd, t_strvect const &split, IRCserv *serv);
void	cmd_list(int fd, t_strvect const &split, IRCserv *serv);
void	cmd_wallops(int fd, t_strvect const &split, IRCserv *serv);
void	cmd_rehash(int fd, t_strvect const &split, IRCserv *serv);
void	cmd_service(int fd, t_strvect const &split, IRCserv *serv);
void	cmd_servlist(int fd, t_strvect const &split, IRCserv *serv);
void	cmd_squery(int fd, const t_strvect &split, IRCserv *serv);

std::string		reply_welcome(IRCserv *serv, Client *client);
std::string		reply_motd(IRCserv *serv, std::string const &it);
std::string		reply_chan_names(IRCserv *serv, Channel *chan, Client *client);
std::string		reply_nochan_visible_names(IRCserv *serv, Client *client);
std::string		reply_lusers(IRCserv *serv, std::string const &target, std::string const &mask = "*");
bool			is_server_registred(const std::string &name, std::string const token, IRCserv *serv);
std::string		getservernamebymask(IRCserv *serv, std::string const &mask);
int				getserverfdbymask(IRCserv *serv, std::string const &mask);
std::string		getnicktoreply(int fd, const t_strvect &split, IRCserv *serv);
std::string		reply_unknowncmd(int fd, const t_strvect &split, IRCserv *serv);

#define CMD_CLIENTONLY	1
#define CMD_SERVERONLY	2

class Command {
private:
	typedef void (*t_command)(int fd, const t_strvect &split, IRCserv *serv);
	t_command	cmd;
	uint		type;
//	message stats
	uint		count;
	size_t		bytes;
	uint		rcount;
public:
	Command();
	~Command();
	Command(t_command cmd);
	Command(Command const &other);
	Command &operator=(Command const &other);

	bool	used(void);
	bool	serveronly(void);
	bool	clientonly(void);
	uint	getcount(void);
	size_t	getbytes(void);
	uint	getrcount(void);
	void	settype(uint type);
	void	Execute(int fd, const t_strvect &split, IRCserv *serv,
		size_t bytes, bool remote);
};

#endif
