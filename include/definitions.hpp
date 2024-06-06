/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   definitions.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <fedmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 17:18:37 by fedmarti          #+#    #+#             */
/*   Updated: 2024/06/06 19:23:43 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <iostream>
#include <list>
#include <map>

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
# define DEFAULT_CONF "test.config"
# define LONGEST_MSG 6 // currently is DELETE. MUST update in case on change
# define CRNL "\r\n"

using std::string;
using std::cout;

class Server;

struct location_t;

typedef std::map<string, string>	conf_t;
typedef	short 						port_t;
typedef	std::pair<int, Server *>	fd_serv_pair;
typedef	std::map<int, Server *>		connections_map;
typedef	std::list<Server *>			serv_list;
typedef	std::list<int>				fd_list;
typedef	std::list<location_t *>		locations_list;
typedef	std::map<int, int>			timeout_fd;

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

# define F_GET 1
# define F_POST 1 << 1
# define F_DELETE 1 << 2
# define F_HEAD 1 << 3
# define F_DIR_LST 1 << 4

typedef	enum	e_req
{
	INVALID = 0,
	GET = F_GET,
	HEAD = F_HEAD,
	POST = F_POST,
	DELETE = F_DELETE,
	FINISH = 3
}				req_t;

typedef enum	e_status_code
{
	CONTINUE = 100,
	SWITCHING_PROTOCOLS,
	PROCESSING,
	EARLY_HINTS,
	OK = 200,
	CREATED,
	ACCEPTED,
	NON_AUTHORTATIVE_INFORMATION,
	NO_CONTENT,
	RESET_CONTENT,
	PARTIAL_CONTENT,
	MULTI_STATUS,
	ALREADY_REPORTED,
	IM_USED = 226,
	MULTIPLE_CHOICES = 300,
	MOVED_PERMANENTLY,
	FOUND,
	SEE_OTHER,
	NOT_MODIFIED,
	USE_PROXY,
	UNUSED,
	TEMPORARY_REDIRECT,
	PERMANENT_READIRECT,
	BAD_REQUEST = 400,
	UNAUTHORIZED,
	PAYMENT_REQUIRED,
	FORBIDDEN,
	NOT_FOUND,
	METHOD_NOT_ALLOWED,
	NOT_ACCEPTABLE,
	PROXY_AUTHENTICATION_REQUIRED,
	REQUEST_TIMEOUT,
	CONFLICT,
	GONE,
	LENGTH_REQUIRED,
	PRECONDITION_FAILED,
	PAYLOAD_TOO_LARGE,
	URI_TOO_LONG,
	UNSUPPORTED_MEDIA_TYPE,
	RANGE_NOT_SATISFIABLE,
	EXPECTATION_FAILED,
	IM_A_TEAPOT,
	MISDIRECTED_REQUEST = 421,
	UNPROCESSABLE_CONTENT,
	LOCKED,
	FAILED_DEPENDENCY,
	TOO_EARLY,
	UPGRADE_REQUIRED,
	PRECONDITION_REQUIRED = 428,
	TOO_MANY_REQUESTS,
	REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
	UNAVAILABLE_FOR_LEGAL_REASONS = 451,
	INTERNAL_SEVER_ERROR = 500,
	NOT_IMPLEMENTED,
	BAD_GATEWAY,
	SERVICE_UNAVAILABLE,
	GATEWAY_TIMEOUT,
	HTTP_VERSION_NOT_SUPPORTED,
	VARIANT_ALSO_NEGOTIATES,
	INSUFFICIENT_STORAGE,
	LOOP_DETECTED,
	NOT_EXTENDED = 510,
	NETWORK_AUTHENTICATION_REQUIRED
} status_code;

void	_print_pool(fd_set &pool, std::string &name);
void	timestamp(string s, colors c = WHITE, textype a = BOLD, bool do_timestamp = true);
string	itoa(int arg);
string	addr_to_str(int addr);
timeval	t_delta_time(timeval &b, timeval &a);
timeval	timeres(int reset);