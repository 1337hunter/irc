/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reply_codes.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: salec <salec@student.21-school.ru>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/21 20:58:56 by salec             #+#    #+#             */
/*   Updated: 2020/11/12 02:09:30 by salec            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REPLY_CODES_HPP
#define REPLY_CODES_HPP

//	"Welcome to the Internet Relay Network <nick>!<user>@<host>"
#define RPL_WELCOME			"001"
//	"Your host is <servername>, running version <ver>"
#define RPL_YOURHOST		"002"
//	"This server was created <date>"
#define RPL_CREATED			"003"
//	"<servername> <version> <available user modes> <available channel modes>"
#define RPL_MYINFO			"004"
/*	- The server sends Replies 001 to 004 to a user upon
	successful registration.	*/

//	"Try server <server name>, port <port number>"
#define RPL_BOUNCE			"005"
/*	- Sent by the server to a user to suggest an alternative
	server.  This is often used when the connection is
	refused because the server is already full.		*/

//	"<channel> <nickname>"
#define RPL_UNIQOPIS		"325"

//	"<channel> <invitemask>"
#define RPL_INVITELIST		"346"
//	"<channel> :End of channel invite list"
#define RPL_ENDOFINVITELIST	"347"
/*	- When listing the 'invitations masks' for a given channel,
	a server is required to send the list back using the
	RPL_INVITELIST and RPL_ENDOFINVITELIST messages.  A
	separate RPL_INVITELIST is sent for each active mask.
	After the masks have been listed (or if none present) a
	RPL_ENDOFINVITELIST MUST be sent.	*/

//	"<channel> <exceptionmask>"
#define RPL_EXCEPTLIST		"348"
//	"<channel> :End of channel exception list"
#define RPL_ENDOFEXCEPTLIST	"349"
/*	- When listing the 'exception masks' for a given channel,
	a server is required to send the list back using the
	RPL_EXCEPTLIST and RPL_ENDOFEXCEPTLIST messages.  A
	separate RPL_EXCEPTLIST is sent for each active mask.
	After the masks have been listed (or if none present)
	a RPL_ENDOFEXCEPTLIST MUST be sent.		*/

//	"You are service <servicename>"
#define RPL_YOURESERVICE	"383"
/*	- Sent by the server to a service upon successful
	registration.	*/

//	Dummy reply number. Not used.
#define RPL_NONE			"300"
/*	":[<reply>{<space><reply>}]"
	or
	":*1<reply> *( " " <reply> )"	*/
#define RPL_USERHOST		"302"
/*	- Reply format used by USERHOST to list replies to
	the query list.  The reply string is composed as
	follows:
	<reply> ::= <nick>['*'] '=' <'+'|'-'><hostname>
	or
	reply = nickname [ "*" ] "=" ( "+" / "-" ) hostname
	The '*' indicates whether the client has registered
	as an Operator.  The '-' or '+' characters represent
	whether the client has set an AWAY message or not
	respectively.	*/

/*	":[<nick> {<space><nick>}]" or
	":*1<nick> *( " " <nick> )"		*/
#define RPL_ISON			"303"
/*	- Reply format used by ISON to list replies to the query list.	*/

//	"<nick> :<away message>"
#define RPL_AWAY			"301"
//	":You are no longer marked as being away"
#define RPL_UNAWAY			"305"
//	":You have been marked as being away"
#define RPL_NOWAWAY			"306"
/*	- These replies are used with the AWAY command (if
	allowed).  RPL_AWAY is sent to any client sending a
	PRIVMSG to a client which is away.  RPL_AWAY is only
	sent by the server to which the client is connected.
	Replies RPL_UNAWAY and RPL_NOWAWAY are sent when the
	client removes and sets an AWAY message.	*/

//	"<nick> <user> <host> * :<real name>"
#define RPL_WHOISUSER		"311"
//	"<nick> <server> :<server info>"
#define RPL_WHOISSERVER		"312"
//	"<nick> :is an IRC operator"
#define RPL_WHOISOPERATOR	"313"
//	"<nick> <integer> :seconds idle"
#define RPL_WHOISIDLE		"317"
//	"<nick> :End of /WHOIS list"
#define RPL_ENDOFWHOIS		"318"
/*	"<nick> :{[@|+]<channel><space>}"
	or
	"<nick> :*( ( "@" / "+" ) <channel> " " )"	*/
#define RPL_WHOISCHANNELS	"319"
/*	Replies 311 - 313, 317 - 319 are all replies
	generated in response to a WHOIS message.  Given that
	there are enough parameters present, the answering
	server must either formulate a reply out of the above
	numerics (if the query nick is found) or return an
	error reply.  The '*' in RPL_WHOISUSER is there as
	the literal character and not as a wild card.  For
	each reply set, only RPL_WHOISCHANNELS may appear
	more than once (for long lists of channel names).
	The '@' and '+' characters next to the channel name
	indicate whether a client is a channel operator or
	has been granted permission to speak on a moderated
	channel.  The RPL_ENDOFWHOIS reply is used to mark
	the end of processing a WHOIS message.	*/

//	"<nick> <user> <host> * :<real name>"
#define RPL_WHOWASUSER		"314"
//	"<nick> :End of WHOWAS"
#define RPL_ENDOFWHOWAS		"369"
/*	- When replying to a WHOWAS message, a server must use
	the replies RPL_WHOWASUSER, RPL_WHOISSERVER or
	ERR_WASNOSUCHNICK for each nickname in the presented
	list.  At the end of all reply batches, there must
	be RPL_ENDOFWHOWAS (even if there was only one reply
	and it was an error).	*/

//	"Channel :Users  Name"
#define RPL_LISTSTART		"321"
//	"<channel> <# visible> :<topic>"
#define RPL_LIST			"322"
//	":End of /LIST" or ":End of LIST"
#define RPL_LISTEND			"323"
/*	- Replies RPL_LISTSTART, RPL_LIST, RPL_LISTEND mark
	the start, actual replies with data and end of the
	server's response to a LIST command.  If there are
	no channels available to return, only the start
	and end reply must be sent.	*/

//	"<channel> <mode> <mode params>"
#define RPL_CHANNELMODEIS	"324"
//	"<channel> :No topic is set"
#define RPL_NOTOPIC			"331"
//	"<channel> :<topic>"
#define RPL_TOPIC			"332"
/*	- When sending a TOPIC message to determine the
	channel topic, one of two replies is sent.  If
	the topic is set, RPL_TOPIC is sent back else
	RPL_NOTOPIC.	*/

//	"<channel> <nick>"
#define RPL_INVITING		"341"
/*	- Returned by the server to indicate that the
	attempted INVITE message was successful and is
	being passed onto the end client.	*/

//	"<user> :Summoning user to IRC"
#define RPL_SUMMONING		"342"
/*	- Returned by a server answering a SUMMON message to
	indicate that it is summoning that user.	*/

//	"<version>.<debuglevel> <server> :<comments>"
#define RPL_VERSION			"351"
/*	- Reply by the server showing its version details.
	The <version> is the version of the software being
	used (including any patchlevel revisions) and the
	<debuglevel> is used to indicate if the server is
	running in "debug mode".
	The "comments" field may contain any comments about
	the version or further version details.		*/

/*	"<channel> <user> <host> <server> <nick>
	<H|G>[*][@|+] :<hopcount> <real name>" or
 	"<channel> <user> <host> <server> <nick>
	( "H" / "G" > ["*"] [ ( "@" / "+" ) ]
	:<hopcount> <real name>"	*/
#define RPL_WHOREPLY		"352"
/*	"<name> :End of /WHO list" or
	"<name> :End of WHO list"	*/
#define RPL_ENDOFWHO		"315"
/*	- The RPL_WHOREPLY and RPL_ENDOFWHO pair are used
	to answer a WHO message.  The RPL_WHOREPLY is only
	sent if there is an appropriate match to the WHO
	query.  If there is a list of parameters supplied
	with a WHO message, a RPL_ENDOFWHO must be sent
	after processing each list item with <name> being
	the item.	*/

/*	"<channel> :[[@|+]<nick> [[@|+]<nick> [...]]]"
	or
	"( "=" / "*" / "@" ) <channel>
	:[ "@" / "+" ] <nick> *( " " [ "@" / "+" ] <nick> )
	- "@" is used for secret channels, "*" for private
	channels, and "=" for others (public channels).	*/
#define RPL_NAMREPLY		"353"
//	"<channel> :End of /NAMES list" or "<channel> :End of NAMES list"
#define RPL_ENDOFNAMES		"366"
/*	- To reply to a NAMES message, a reply pair consisting
	of RPL_NAMREPLY and RPL_ENDOFNAMES is sent by the
	server back to the client.  If there is no channel
	found as in the query, then only RPL_ENDOFNAMES is
	returned.  The exception to this is when a NAMES
	message is sent with no parameters and all visible
	channels and contents are sent back in a series of
	RPL_NAMEREPLY messages with a RPL_ENDOFNAMES to mark
	the end.	*/

//	"<mask> <server> :<hopcount> <server info>"
#define RPL_LINKS			"364"
//	"<mask> :End of /LINKS list" or "<mask> :End of LINKS list"
#define RPL_ENDOFLINKS		"365"
/*	- In replying to the LINKS message, a server must send
	replies back using the RPL_LINKS numeric and mark the
	end of the list using an RPL_ENDOFLINKS reply.	*/

//	"<channel> <banid>"
#define RPL_BANLIST			"367"
//	"<channel> :End of channel ban list"
#define RPL_ENDOFBANLIST	"368"
/*	- When listing the active 'bans' for a given channel,
	a server is required to send the list back using the
	RPL_BANLIST and RPL_ENDOFBANLIST messages.  A separate
	RPL_BANLIST is sent for each active banid.  After the
	banids have been listed (or if none present) a
	RPL_ENDOFBANLIST must be sent.	*/


//	":<string>"
#define RPL_INFO			"371"
//	"<channel> :Channel key already set"
#define RPL_ENDOFINFO		"374"
/*	- A server responding to an INFO message is required to
	send all its 'info' in a series of RPL_INFO messages
	with a RPL_ENDOFINFO reply to indicate the end of the
	replies.	*/

//	"<server> Message of the day"
#define RPL_MOTDSTART		"375"
//	"<text>"
#define RPL_MOTD			"372"
//	"End of MOTD command"
#define RPL_ENDOFMOTD		"376"
/*	- When responding to the MOTD message and the MOTD file
	is found, the file is displayed line by line, with
	each line no longer than 80 characters, using
	RPL_MOTD format replies.  These should be surrounded
	by a RPL_MOTDSTART (before the RPL_MOTDs) and an
	RPL_ENDOFMOTD (after).	*/

//	":You are now an IRC operator"
#define RPL_YOUREOPER		"381"
/*	- RPL_YOUREOPER is sent back to a client which has
	just successfully issued an OPER message and gained
	operator status.	*/

//	"<config file> :Rehashing"
#define RPL_REHASHING		"382"
/*	- If the REHASH option is used and an operator sends
	a REHASH message, an RPL_REHASHING is sent back to
	the operator.	*/

//	"<server> :<string showing server's local time>"
#define RPL_TIME			"391"
/*	- When replying to the TIME message, a server must send
	the reply using the RPL_TIME format above.  The string
	showing the time need only contain the correct day and
	time there.  There is no further requirement for the
	time string.	*/

//	":UserID   Terminal  Host"
#define RPL_USERSSTART		"392"
//	":<username> <ttyline> <hostname>" -> ":%-8s %-9s %-8s"
#define RPL_USERS			"393"
//	":End of users"
#define RPL_ENDOFUSERS		"394"
//	":Nobody logged in"
#define RPL_NOUSERS			"395"
/*	- If the USERS message is handled by a server, the
	replies RPL_USERSTART, RPL_USERS, RPL_ENDOFUSERS and
	RPL_NOUSERS are used.  RPL_USERSSTART must be sent
	first, following by either a sequence of RPL_USERS
	or a single RPL_NOUSER.  Following this is
	RPL_ENDOFUSERS.		*/


/*	"Link <version & debug level> <destination> <next server>"
	or
	"Link <version & debug level> <destination>
	<next server> V<protocol version>
	<link uptime in seconds> <backstream sendq>
	<upstream sendq>"	*/
#define RPL_TRACELINK		"200"
//	"Try. <class> <server>"
#define RPL_TRACECONNECTING	"201"
//	"H.S. <class> <server>"
#define RPL_TRACEHANDSHAKE	"202"
//	"???? <class> [<client IP address in dot form>]"
#define RPL_TRACEUNKNOWN	"203"
//	"Oper <class> <nick>"
#define RPL_TRACEOPERATOR	"204"
//	"User <class> <nick>"
#define RPL_TRACEUSER		"205"
/*	"Serv <class> <int>S <int>C <server> <nick!user|*!*>@<host|server>"
	or
	"Serv <class> <int>S <int>C <server> <nick!user|*!*>@<host|server>
	V<protocol version>"	*/
#define RPL_TRACESERVER		"206"
//	"Service <class> <name> <type> <active type>"
#define RPL_TRACESERVICE	"207"
//	"<newtype> 0 <client name>"
#define RPL_TRACENEWTYPE	"208"
//	"Class <class> <count>"
#define RPL_TRACECLASS		"209"
//	Unused.
#define RPL_TRACERECONNECT	"210"
//	"File <logfile> <debug level>"
#define RPL_TRACELOG		"261"
//	"<server name> <version & debug level> :End of TRACE"
#define RPL_TRACEEND		"262"
/*	- The RPL_TRACE* are all returned by the server in
		response to the TRACE message.  How many are
		returned is dependent on the the TRACE message and
		whether it was sent by an operator or not.  There
		is no predefined order for which occurs first.
		Replies RPL_TRACEUNKNOWN, RPL_TRACECONNECTING and
		RPL_TRACEHANDSHAKE are all used for connections
		which have not been fully established and are either
		unknown, still attempting to connect or in the
		process of completing the 'server handshake'.
		RPL_TRACELINK is sent by any server which handles
		a TRACE message and has to pass it on to another
		server.  The list of RPL_TRACELINKs sent in
		response to a TRACE command traversing the IRC
		network should reflect the actual connectivity of
		the servers themselves along that path.
		RPL_TRACENEWTYPE is to be used for any connection
		which does not fit in the other categories but is
		being displayed anyway.		*/

/*	"<linkname> <sendq> <sent messages> <sent bytes> <received messages>
	<received bytes> <time open>"
	or
	"<linkname> <sendq> <sent messages>
	<sent Kbytes> <received messages>
	<received Kbytes> <time open>"	*/
#define RPL_STATSLINKINFO	"211"
/*	- reports statistics on a connection.  <linkname>
	identifies the particular connection, <sendq> is
	the amount of data that is queued and waiting to be
	sent <sent messages> the number of messages sent,
	and <sent Kbytes> the amount of data sent, in
	Kbytes. <received messages> and <received Kbytes>
	are the equivalent of <sent messages> and <sent
	Kbytes> for received data, respectively.  <time
	open> indicates how long ago the connection was
	opened, in seconds.		*/

//	"<command> <count>"
#define RPL_STATSCOMMANDS	"212"
/*	- reports statistics on commands usage.	*/

//	"C <host> * <name> <port> <class>"
#define RPL_STATSCLINE		"213"
//	"N <host> * <name> <port> <class>"
#define RPL_STATSNLINE		"214"
//	"I <host> * <host> <port> <class>"
#define RPL_STATSILINE		"215"
//	"K <host> * <username> <port> <class>"
#define RPL_STATSKLINE		"216"
//	"Y <class> <ping frequency> <connect frequency> <max sendq>"
#define RPL_STATSYLINE		"218"
/*	"<stats letter> :End of /STATS report"
	or
	"<stats letter> :End of STATS report"	*/
#define RPL_ENDOFSTATS		"219"
//	"L <hostmask> * <servername> <maxdepth>"
#define RPL_STATSLLINE		"241"
//	":Server Up %d days %d:%02d:%02d"
#define RPL_STATSUPTIME		"242"
/*	- reports the server uptime.	*/

//	"O <hostmask> * <name>"
#define RPL_STATSOLINE		"243"
/*	- reports the allowed hosts from where user may become IRC operators.	*/

//	"H <hostmask> * <servername>"
#define RPL_STATSHLINE		"244"
//	"<user mode string>"
#define RPL_UMODEIS			"221"
/*	- To answer a query about a client's own mode, RPL_UMODEIS is sent back. */

/*	":There are <integer> users and <integer> invisible on <integer> servers"
	or
	":There are <integer> users and <integer> services on <integer> servers" */
#define RPL_LUSERCLIENT		"251"
//	"<integer> :operator(s) online"
#define RPL_LUSEROP			"252"
//	"<integer> :unknown connection(s)"
#define RPL_LUSERUNKNOWN	"253"
//	"<integer> :channels formed"
#define RPL_LUSERCHANNELS	"254"
//	":I have <integer> clients and <integer> servers"
#define RPL_LUSERME			"255"
/*	- In processing an LUSERS message, the server
	sends a set of replies from RPL_LUSERCLIENT,
	RPL_LUSEROP, RPL_USERUNKNOWN,
	RPL_LUSERCHANNELS and RPL_LUSERME.  When
	replying, a server must send back
	RPL_LUSERCLIENT and RPL_LUSERME.  The other
	replies are only sent back if a non-zero count
	is found for them.		*/

//	"<server> :Administrative info"
#define RPL_ADMINME			"256"
//	":<admin info>"
#define RPL_ADMINLOC1		"257"
// ":<admin info>"
#define RPL_ADMINLOC2		"258"
// ":<admin info>"
#define RPL_ADMINEMAIL		"259"
/*	- When replying to an ADMIN message, a server
		is expected to use replies RLP_ADMINME
		through to RPL_ADMINEMAIL and provide a text
		message with each.  For RPL_ADMINLOC1 a
		description of what city, state and country
		the server is in is expected, followed by
		details of the university and department
		(RPL_ADMINLOC2) and finally the administrative
		contact for the server (an email address here
		is required) in RPL_ADMINEMAIL.		*/

//	"<name> <server> <mask> <type> <hopcount> <info>"
#define RPL_SERVLIST		"234"
//	"<mask> <type> :End of service listing"
#define RPL_SERVLISTEND		"235"
/*	- When listing services in reply to a SERVLIST message,
	a server is required to send the list back using the
	RPL_SERVLIST and RPL_SERVLISTEND messages.  A separate
	RPL_SERVLIST is sent for each service.  After the
	services have been listed (or if none present) a
	RPL_SERVLISTEND MUST be sent.	*/

//	"<command> :Please wait a while and try again."
#define RPL_TRYAGAIN		"263"
/*	- When a server drops a command without processing it,
	it MUST use the reply RPL_TRYAGAIN to inform the
	originating client.		*/

//	reserved
#define RPL_STATSQLINE		"217"
//	reserved
#define RPL_SERVICEINFO		"231"
//	reserved
#define RPL_ENDOFSERVICES	"232"
//	reserved
#define RPL_SERVICE			"233"
//	reserved
#define RPL_WHOISCHANOP		"316"
//	reserved
#define RPL_KILLDONE		"361"
//	reserved
#define RPL_CLOSING			"362"
//	reserved
#define RPL_CLOSEEND		"363"
//	reserved
#define RPL_INFOSTART		"373"
//	reserved
#define RPL_MYPORTIS		"384"
//	reserved
#define RPL_STATSCLINE		"213"
//	reserved
#define RPL_STATSNLINE		"214"
//	reserved
#define RPL_STATSILINE		"215"
//	reserved
#define RPL_STATSKLINE		"216"
//	reserved
#define RPL_STATSQLINE		"217"
//	reserved
#define RPL_STATSYLINE		"218"
//	reserved
#define RPL_STATSVLINE		"240"
//	reserved
#define RPL_STATSLLINE		"241"
//	reserved
#define RPL_STATSSLINE		"244"
//	reserved
#define RPL_STATSPING		"246"
//	reserved
#define RPL_STATSBLINE		"247"
//	reserved
#define RPL_STATSDLINE		"250"

#endif
