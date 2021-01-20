# ft_irc - a School 21 (42 Network) project

### Developed by
- Antip003 aka gbright
- thefullarcticfox aka salec

### Reference material
RFC 1459, RFC 2810, RFC 2811, RFC 2812, RFC 2813, RFC 7194 and different irc implementations
> IRCv3 features are not implemented

Requirements
------------
- Unix-like OS (tested on Arch Linux, Ubuntu and Mac OS Mojave)
- `clang++` to build everything
- `tar` and `clang` to build OpenSSL library
- `qt5` and `qmake` to build client (not tested for Mac OS)
> Compiler support for C++11 features is also required if you want to use them by disabling C++98 compability mode

Build and Run
-------------
- Run `./Config` for Linux or `./Config_darwin` for Mac OS and follow the prompts
- Run `make` to run build
- Run `make gencert` to generate a self-signed certificate with key if you want to enable secure connections with TLS
- Run `./ircserv 6667`

Configuration
-------------
Use `./conf/ircserv.conf` to configure different listening ports, server links, operators, etc

If you want a debug build or change defined constants - use Config scripts and follow the prompts then rebuild the project

You can also configure `ircserv` to use C++11 features including `unordered_map` by choosing 'No' for C++98 support in Config scripts

Server command line arguments
-----------------------------
`./ircserv [host:port_network:password_network] <port> <password>`
- `<port>` - [required] main server port (for tls port configure listening port in `./conf/ircserv.conf`)
- `<password>` - [optional] server password
- `[host:port_network:password_network]` - [optional] to connect to another ircserv server on launch (must be in conf file)
> Better use CONNECT command as a server operator instead of passing server as a command line argument

> For ircbot command line arguments run `./ircbot -h`

Installation
------------
Run `make install`

Bonus stuff
-----------
You can build irc bot and irc client by running `make bonus`

Or just run `make` in `bot` and `client` directories

Used libraries and APIs
-----------------------
- `ircserv` uses OpenSSL library v1.1.1h
- `client` uses OpenSSL library v1.1.1h and Qt5
- `ircbot` uses OpenWeatherMap API for current weather info (you will need to change the APIKEY if you want to test it)