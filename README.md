# IRC server 2.10

### Developed by
- Antip003
- thefullarcticfox

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
- Run `./ircserv`

Configuration
-------------
Use `./conf/ircserv.conf` to configure different listening ports, server links, operators, etc

If you want a debug build or change defined constants - use `Config` scripts and follow the prompts then run `make`

You can also configure `ircserv` to use C++11 features including `unordered_map` by choosing 'No' for C++98 support in `Config` scripts

Command line arguments
----------------------
None for `ircserv` and `client`

Run `./ircbot -h` to list `ircbot` command line arguments

Installation
------------
Run `make install`

Bonus stuff
-----------
You can build irc bot and irc client by running `make bonus`

Or just run `make` in `./bot` directory and `qmake && make` in `./client` directory

Used libraries and APIs
-----------------------
- `ircserv` uses OpenSSL library v1.1.1h
- `client` uses OpenSSL library v1.1.1h and Qt5
- `ircbot` uses OpenWeatherMap API for current weather info
> you will need to change the `WEATHERAPIKEY` if you want weather command to work in `ircbot`
