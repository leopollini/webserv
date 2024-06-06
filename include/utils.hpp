/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 13:29:37 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/06 10:48:37 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP

# define UTILS_HPP

# include <sstream>
# include <iostream>
# include <string>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/time.h>
# include <csignal>
# include <unistd.h>
# include <list>
# include <map>
# include <deque>
# include <cmath>
# include "utils.hpp"
# include "parsing_util.hpp"
# include <fstream>
# include <algorithm>

# define ERROR RED // do not expand
# define WARNING YELLOW
# define DEBUG CYAN
# define INFO WHITE
# define DONE GREEN
# define CONNECT MAGENTA
# define REC_MSG_PRNT GRAYI,COLORII

# define DOWN_SERVER_TRIES_MAX 20
# define DOWN_SERVER_SLEEP_MS 2000
# define DOWN_SERVER_MAX 20
# define NO_SERVER_SLEEP_TIME_MS 30000
# define MAX_CONNECTIONS 100
# define DEFAULT_CONF "lolfile"
# define LONGEST_MSG 6 // currently is DELETE. MUST update in case on change

using std::string;
using std::cout;

class Server;

typedef std::map<string, string>	conf_t;
typedef	short 						port_t;
typedef	std::pair<int, Server *>	fd_serv_pair;
typedef	std::map<int, Server *>		connections_map;
typedef	std::list<Server *>			serv_list;
typedef	std::list<int>				fd_list;

typedef	enum	e_colors
{
	GRAYI = 0,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	WHITE,
	GRAYII,
	GRAYIII,
}				colors;

typedef	enum	e_text
{
	WHITE_O = 0,
	BOLD,
	FADE,
	ITALIC,
	UNDERLINED,
	COLORI,
	COLORII,
	BACKGROUND,
	DISAPPEAR_LOL,
	CROSSED
}				textype;

typedef	enum	e_req
{
	INVALID = 0,
	FINISH,
	GET,
	POST,
	DELETE,
	HEAD
}				req_t;

void	_print_pool(fd_set &pool, std::string &name);
void	timestamp(string s, colors c = WHITE, textype a = BOLD, bool do_timestamp = true);
string	itoa(int arg);
string	addr_to_str(int addr);
timeval	t_delta_time(timeval &b, timeval &a);
timeval	timeres(int reset);

#endif
