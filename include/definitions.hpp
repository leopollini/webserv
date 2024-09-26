/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   definitions.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/06 17:18:37 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/26 18:03:45 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <iostream>
#include <list>
#include <map>
#include <set>

// 0: no debug info; 1: show debug info
# define DEBUG_INFO 0

# define ERROR RED
# define WARNING YELLOW
# define DEBUG CYAN
# define INFO WHITE
# define DONE GREEN
# define CONNECT MAGENTA
# define REC_MSG_PRNT GRAYI
# define REC_MSG_PRNT GRAYI
# define CRNL "\r\n"
# define DCRNL "\r\n\r\n"

# define SERVER_RETRY_TIME 200000
# define SHORT_REVIVE_TIME 20000
# define DOWN_SERVER_TRIES_MAX 20
# define DOWN_SERVER_SLEEP_MS 2000
# define DOWN_SERVER_MAX 20
# define NO_SERVER_SLEEP_TIME_MS 30000
# define MAX_CONNECTIONS 1
# define DEFAULT_RETURN_RESCODE 200
# define CONNECTION_TIMEOUT 30 // measured in secs
# define DEFAULT_CONF "test.config"
# define DEFAULT_PROTOCOL "1.0"

# define H_BODY_SIZE "Content-Length"
# define H_TRANSFER_ENCODING "Transfer-encoding"
# define SERVER_DEFAULT_PORT "8080"
# define SERVER_DEFAULT_ROOT "."
# define SERVER_DEFAULT_NAME "default_server_name"
# define DEFAULT_MOVED_FILE "redirect_file.html"
# define DEFAULT_AUTOINDEX_CGI_DIR "dir_list_cgi.sh"
# define DEFAULT_INDEX_FILE "index.html"
# define DEFAULT_DELETE_CGI "delete_cgi.sh"
# define DEFAULT_ERRPGS_DIR "error_pages"
# define DEFAULT_MAX_BODY_SIZE "100000000"
# define DEFAULT_LOCATION_ALLOWS F_GET | F_POST | F_DELETE | F_HEAD | F_DIR_LST // all methods allowed
# define DEFAULT_MAX_BODY_SIZE "100000000"
# define DEFAULT_LOCATION_ALLOWS F_GET | F_POST | F_DELETE | F_HEAD | F_DIR_LST // all methods allowed
# define REDIR_URL(s) "<head>\n<meta http-equiv=\"Refresh\" content=\"0; URL=" + s + "\" />\n</head>"

# define EXTRA_ENV_SPACE 25

# define SUCCESSFUL_POST_PAGE "<head>Posted successfully.</head>"
# define EXTRA_ENV_SPACE 25

# define SUCCESSFUL_POST_PAGE "<head>Posted successfully.</head>"

# define SAY(x) if (DEBUG_INFO) cout << x


using std::string;
using std::cout;

class Server;

struct location_t;

typedef std::map<string, string>	conf_t;
typedef std::map<short, string>		ecode_t;
typedef	short 						port_t;
typedef std::set<port_t>			ports_set_t;
typedef	std::list<Server *>			serv_list_t;
typedef	std::map<int, Server *>		connections_map_t;
typedef	std::map<int, serv_list_t>	port_servs_map_t;
typedef	std::list<int>				fd_list_t;
typedef	std::list<location_t *>		locations_list_t;
typedef	std::map<int, long>			timeout_fd_t;
typedef	std::set<string>			str_set_t;
typedef std::set<pid_t>				pidlst_t;
typedef std::map<int, string>		fd_msg_map;


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
	FINISH = 3,
	INCOMPLETE = 5,
	LOLDEF
}				req_t;

typedef enum	e_status_code
{
	_ZERO = 0,
	_REQUEST_DIR_LISTING = 1,
	_CGI_RETURN,
	_DONT_SEND,
	_REQUEST_DELETE,
	_POST_SUCCESS,
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
	PERMANENT_REDIRECT,
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
} status_code_t;

// void	_print_pool(fd_set &pool, std::string &name);
// void	_print_pool(fd_set &pool, std::string &name);
void	timestamp(string s, colors c = WHITE, textype a = BOLD, bool do_timestamp = true);
string	itoa(int arg);
string	addr_to_str(int addr);
timeval	t_delta_time(timeval &b, timeval &a);
timeval	timeres(int reset);

//does the file/directory exist
# define C_OK(x) (x != 0)
//(x & C_READ) read access?
# define C_READ 1
//(x & C_WRITE) write access?
# define C_WRITE 1 << 1
//(x & C_EXEC) execution access?
# define C_EXEC 1 << 2
//(x & C_FILE) is it a file?
# define C_FILE 1 << 3
//(x & C_DIR) is it a directory?
# define C_DIR 1 << 4

//controls if file exists if it's a directory, and sets rwx flags:
//C_OK(c) exists?
//(c & C_READ) read access..
char	checkCharacteristics(const char *path);
char	checkFastAccess(const char *path);
char	checkDir(const char *path);
bool	isOkToSend(char flags);
bool	exists(char flags);
char	read_allows(string &allow);
req_t	request_method(string request_line);
//returns string according to type
string	request_type_str(req_t type);
string	strip(string str, string charset);

struct request_t;
void	printHttpRequest(request_t &request, std::ostream &out = std::cout);