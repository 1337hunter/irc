#include "ircserv.hpp"
#include "tools.hpp"
#include "message.hpp"

/*
	Command: SERVICE
	Parameters: <servicename> <servertoken> <distribution> <type>
				<hopcount> <info>

	The SERVICE command is used to introduce a new service.  This form of
	the SERVICE message SHOULD NOT be allowed from client (unregistered,
	or registered) connections.  However, it MUST be used between servers
	to notify other servers of new services joining the IRC network.

	The <servertoken> is used to identify the server to which the service
	is connected.  (See section 4.1.2 for more information on server
	tokens).

	The <hopcount> parameter is used by servers to indicate how far away
	a service is from its home server.  A local connection has a hopcount
	of 0.  The hopcount value is incremented by each passed server.

	The <distribution> parameter is used to specify the visibility of a
	service.  The service may only be known to servers which have a name
	matching the distribution.  For a matching server to have knowledge
	of the service, the network path between that server and the server
	to which the service is connected MUST be composed of servers whose
	names all match the mask.  Plain "*" is used when no restriction is
	wished.
	The <type> parameter is currently reserved for future usage.

	Numeric Replies:
		ERR_ALREADYREGISTRED
		ERR_NEEDMOREPARAMS
		ERR_ERRONEUSNICKNAME
		RPL_YOURESERVICE
		RPL_YOURHOST
		RPL_MYINFO

	Example:
	SERVICE dict@irc.fr 9 *.fr 0 1 :French Dictionary r" registered
		on server "9" is being announced to another server.
		This service will only be available on servers whose
		name matches "*.fr".

	Upon successfully registering a new service connection, the server is
	subject to the same kind of REQUIREMENTS as for a user.  Services
	being somewhat different, only the following replies are sent:
	RPL_YOURESERVICE, RPL_YOURHOST, RPL_MYINFO.

	After dealing with this, the server MUST then send out to other
	servers (SERVICE message) the new service's nickname and other
	information as supplied by the service (SERVICE message) and as the
	server could discover (from DNS servers).

   Each service is distinguished from other services by a service name
   composed of a nickname and a server name.  The nickname has a maximum
   length of nine (9) characters. See the protocol grammar rules
   (section 3.3.1) for what may and may not be used in a nickname. The
   server name used to compose the service name is the name of the
   server to which the service is connected. In addition to this
   service name all servers MUST know the service type.

   Services differ from users by the format of their identifier, but
   more importantly services and users don't have the same type of
   access to the server: services can request part or all of the global
   state information that a server maintains, but have a more restricted
   set of commands available to them (See "IRC Client Protocol" [IRC-
   CLIENT] for details on which) and are not allowed to join channels.
   Finally services are not usually subject to the "Flood control"
   mechanism described in section 5.8.
*/

void	cmd_service(int fd, t_strvect const &split, IRCserv *serv)
{
	fd = 0; (void)split; serv = 0;
}
