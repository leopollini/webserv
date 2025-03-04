/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/01/21 14:07:31 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../include/Server.hpp"
#include "../include/Responser.hpp"
#include "Webserv.hpp"
#include <poll.h>

Server::Server(short id) : _clientfds(), _id(id), _state(0), _current_request(), _resp(this), _lastUpAttempt (0), _down_count(0), _is_sharing_port(false)
{
	// _received_head[BUFFER_SIZE] = 0;
	timestamp("Added new Server! Id: " + itoa(_id) + "!\n", CYAN);
	_env[LOC_ROOT] = SERVER_DEFAULT_ROOT;
	_env[NAME] = SERVER_DEFAULT_NAME;
}

Server::~Server()
{
	down();
	if (_sock.sock >= 0)
		close(_sock.sock);
	if (_sock.sock >= 0)
		close(_sock.fd);
	for (fd_list_t::iterator i = _clientfds.begin(); i != _clientfds.end(); i++)
		if (_sock.sock >= 0)
			close(*i);
	for (locations_list_t::iterator i = _loc_ls.begin(); i != _loc_ls.end(); i++)
		delete *i;
	timestamp("Server " + itoa(_id) + " removed!\n", BLUE);
}

int	Server::Accept()
{
	int	t = _sock.Accept();

	// _clientfds.push_front(t);
	// _current_request = request_t();
	// getsockname(_clientfds.front(), (sockaddr *)&_sock.client, &_sock.len);
	// SAY("New client at IP: " << addr_to_str(_sock.client.sin_addr.s_addr) << "!\n");
	// fcntl(_clientfds.front(), F_SETFL, fcntl(_clientfds.front(), F_GETFL, 0) | O_NONBLOCK);
	// return _clientfds.front();

	_current_request = request_t();
	getsockname(t, (sockaddr *)&_sock.client, &_sock.len);
	SAY("New client at IP: " << addr_to_str(_sock.client.sin_addr.s_addr) << "!\n");
	fcntl(t, F_SETFL, fcntl(t, F_GETFL, 0) | O_NONBLOCK);
	return t;
}

void	Server::addLocation(location_t *l)
{
	l->dir = l->stuff[L_DIR];
	if (l->dir.empty())
		throw EmptyLocationDir();
	l->allows = read_allows(l->stuff[L_ALLOW_METHODS]);

	string	&line = l->dir;

	if (line.find(' ') != string::npos)
	{
		l->allowed_extensions.insert(line.substr(line.find(' ') + 1));
		line.erase(line.find(' '));
	}
	l->stuff[L_DIR] = line;
	// This allows extension precedence. Extension match has precedence over directory match
	l->allowed_extensions.empty() ? _loc_ls.push_back(l) : _loc_ls.push_front(l);
}

//tries to raise the server (opening its socket) after retry_time (SERVER_RETRY_TIME by default) 
bool Server::tryUp(time_t retry_time)
{
	_state = 0;
	time_t now = time(NULL);
	if (_is_sharing_port == -1 || now - _lastUpAttempt < retry_time) //wait more to retry
		return (false);
	if (!_state)
		up();
	return (_state);
	return (_state);
}

void Server::up()
{
	if (_env[PORT].empty())
		_env[PORT] = SERVER_DEFAULT_PORT;
	if (BetterSelect::_used_ports.count(getPort()))
	{
		Server	*t = BetterSelect::_used_ports[getPort()].front();
		t->_is_sharing_port = 1;
		_is_sharing_port = -1;
		if (!t->_state)
			throw SharedPortOccupied();
		if (!t->_state)
			throw SharedPortOccupied();
		getSockFd() = t->getSockFd();
	}
	else
		_sock.init(atoi(_env[PORT].c_str()));
	BetterSelect::_used_ports[getPort()].push_back(this);
	timestamp("Server " + itoa(_id) + " at port " + _env[PORT] + " now open!\n", CYAN);
	_state = 1;
}

void Server::down()
{
	if (!_state)
		return;
	_sock.down();
	timestamp("Server " + itoa(_id) + ": Port " + _env[PORT] + " now closed!\n", YELLOW);
	_state = 0;
}

void Server::closeConnection(int fd)
{
	timestamp("Server " + itoa(_id) + ": Closing fd: " + itoa(fd) + "!\n", INFO);
	close(fd);
}

void Server::printHttpRequest(string &msg, int fd_from)
{
	timestamp("Server " + itoa(_id) + ": Received from connection at fd " + itoa(fd_from) + ":\n\t" + msg + "\n", REC_MSG_PRNT);
}

void Server::HttpRequestLog(string &request_line, int fd_from)
{
	timestamp("Server " + itoa(_id) + ": Received from connection at fd " + itoa(fd_from) + ":\n\t" + request_line + "\n", REC_MSG_PRNT);
}

status_code_t	Server::manageDir()
{
	string 	index_file;

	if (getEnv(L_AUTOINDEX, _resp.getLoc()) == "yes")		// autoindex on
		return _REQUEST_DIR_LISTING;

	index_file = (_current_request.type == GET || _current_request.type == HEAD ?\
			getEnv(L_INDEX, _current_request.loc) : "");	// prevents deleting/overwriting index file involuntarly
	if (index_file.empty())									// no index file specified
		return FORBIDDEN;

	if (*--_resp.getDir().end() != '/')
		_resp.getDir() += '/';

	index_file = _resp.getDir() + index_file; 				//searches for index files
	char	index_flags = checkCharacteristics(index_file.c_str());

	if (isOkToSend(index_flags))							// found a valid index file
	{
		_resp.getDir() = index_file;
		_resp.getFileFlags() = index_flags;
	}
	else
		return FORBIDDEN;
	return _ZERO;
}

// If nothing is found, returns ""
string	&Server::getEnv(string key, location_t *location)
{
	conf_t::iterator var;

	// Look inside location directive
	if (location)
	{
		var = location->stuff.find(key);
		if (var != location->stuff.end() && !var->second.empty())
			return (var->second);
	}
	
	// Look inside server directive
	var = _env.find(key);
	
	if (var != _env.end())
		return (var->second);

	// Look inside http directive
	return (Webserv::getInstance().getEnv(key));
}

//	################ Here are other function definitions of classes that depend to Server / to which Server is dependant. Sorry i could not create a separate file ):
void	request_t::littel_parse(Server *s)
{
	if (loc)
		uri = uri.substr(loc->dir.size());
	else
		SAY("LOCATION NOT FOUND!\n");
	if (uri[0] != '/')
		uri = '/' + uri;
	uri = s->getEnv(LOC_ROOT, loc) + uri;
	SAY("requested uri be: \'" << uri << "\'\n");
}

#define QUERY_STR_ENV "QUERY_STRING"
#define CONTENT_SIZE "CONTENT_LENGTH"

static void handle_meta_variables(Server *s, string &query_string, string &body, BetterEnv &env)
{
	if (!query_string.empty())
		env.addVariable(QUERY_STR_ENV, query_string);
	if (body.size())
		env.addVariable(CONTENT_SIZE, itoa(body.size()));
	

	env.removeVariable("PWD");

	(void)s;
}

static size_t next_slash(string &str)
{
	size_t next = str.find('/');

	// if (next == string::npos)
		// return (next);

	while (next != string::npos &&  (next == 0 || str[next + 1] == '/' || str[next - 1] == '.'))
	{
		next = str.find('/', next + 1);
	}
	return (next);
}


static void location_fuckery(string &cgi_dir, string &uri_dir, string &backtrack_path, Server *s)
{
	size_t first_slash = next_slash(uri_dir);
	(void)s;

	while (first_slash != string::npos)
	{ 
		uri_dir = uri_dir.substr(first_slash + 1);
		backtrack_path.append("../");
		first_slash = next_slash(uri_dir);
	}
	if (uri_dir == "" || uri_dir == "/")
		uri_dir = ".";

	if (cgi_dir[0] != '/')
		cgi_dir = backtrack_path + cgi_dir;	
}



// CGI function!!!
void	CGIManager::start(Server *s, const string &cgi_dir, const string &uri_dir, string query_string, string body)
{
	BetterEnv					env(_envp);
	std::vector<const char *>	args;
	pid_t						fk;
	int							t[2];
	int							pipefd[2];

	SAY("Trying to execute " << cgi_dir << "; args: " << uri_dir << '\n');


	string root = s->getEnv("root", s->getCurrentRequest().loc).substr();

	if (root == ".")
		root.append("/");
	string stripped_requested_object = root;
	if (uri_dir != "./")
		stripped_requested_object += uri_dir;
	string path_to_binary = cgi_dir.substr(0);
	string original_uri = uri_dir.substr(0, uri_dir.find_last_of('/') + 1);
	cout << "\n";
	string backtrack_path = "./";
	pipe(pipefd);
	if (!(fk = fork()))
	{
		location_fuckery(path_to_binary, stripped_requested_object, backtrack_path, s);
		if (chdir(original_uri.c_str()))
		{
			Webserv::_up = -1;
			std::cerr << "A CGI crashed!!! Couldn't cd into " << original_uri << std::endl;
			close (pipefd[0]);
			close (pipefd[1]);
			close(STDOUT_FILENO);
			return ;
		}

		handle_meta_variables(s, query_string, body, env);

		close (pipefd[1]);
		args.push_back(path_to_binary.c_str());
		args.push_back(stripped_requested_object.c_str());
		args.push_back(NULL);

		t[1] = dup(STDOUT_FILENO);
		t[0] = dup(STDIN_FILENO);
		dup2(s->getFd(), STDOUT_FILENO);
		dup2(pipefd[0], STDIN_FILENO);
		if (DEBUG_INFO)
			std::cerr << "### calling " + string(args[0]) + ".\n";
		execve(args[0], (char *const*)args.data(), env.c_envp());
		dup2(t[1], STDOUT_FILENO);
		dup2(t[0], STDIN_FILENO);
		if (chdir (backtrack_path.c_str()))
			std::cerr << "couldn't cd back into original working directory" << std::endl;
		close(STDOUT_FILENO);
		close(pipefd[0]);
		close(STDIN_FILENO);
		close(t[0]);
		close(t[1]);
		
		Webserv::_up = -1;
		std::cerr << "A CGI crashed!!! Called as \'" << cgi_dir << "\'\n";
		return ;
	}
	timestamp("CGI started!!\n");
	usleep(1000);
	close(pipefd[0]);

	close(s->getFd());

	if (!body.empty())
		Webserv::getInstance().superPipeSet(pipefd[1], body);
	else
		close(pipefd[1]);
	_pids[fk] = (std::pair<pid_t, time_t>(s->getFd(), time(NULL)));
}

void	CGIManager::purgeCGI()
{
	for (pid_time_lst::iterator i = _pids.begin(); i != _pids.end();)
	{
		if (i->second.second < time(NULL) - CGI_TIMEOUT)
		{
			if (!kill(i->first, SIGKILL))
				SAY("closing cgi for timeout\n");
			_pids.erase(i++);
			continue ;
		}
		++i;
	}
}

Responser &Responser::operator=(const request_t &t)
{
	_dir = t.uri;
	_loc = t.loc;
	return *this;
}

void	Responser::clear()
{
	_head.clear();
	_body.clear();
	_dir.clear();
	_extra_args.clear();
	_is_returning = 0;
}

char	Responser::internalServerError()
{
	SAY("Preparing 500 response.\n");
	_dir = _serv->getEnv(E_500, _loc);
	_body = Parsing::read_file(_dir);
	_res_code = INTERNAL_SERVER_ERROR;
	return 0;
}

// look for better method. Big switch uglyyy :((
char	Responser::buildResponseBody()
{
	//check file existance
	try
	{
		switch (_res_code)
		{
		case OK :
			SAY("Reading file\n");
		 break ;
		case BAD_REQUEST :
			_dir = _serv->getEnv(E_400, _loc);
			SAY("Looking for 400 response" << _dir << "'\n");
		 break ;
		case NOT_FOUND :
			_dir = _serv->getEnv(E_404, _loc);
			SAY("Looking for 404 response" << _dir << "'\n");
		 break ;
		case FORBIDDEN :
			_dir = _serv->getEnv(E_403, _loc);
			SAY("Looking for 403 response" << _dir << "'\n");
		 break ;
		case METHOD_NOT_ALLOWED :
			_dir = _serv->getEnv(E_405, _loc);
			SAY("Looking for 405 response" << _dir << "'\n");
		 break ;
		case INTERNAL_SERVER_ERROR :
			_dir = _serv->getEnv(E_500, _loc);
			SAY("Looking for 500 response" << _dir << "'\n");
		 break ;
		case PAYLOAD_TOO_LARGE :
			_dir = _serv->getEnv(E_413, _loc);
			SAY("Looking for 413 response" << _dir << "'\n");
		 break ;
		case MOVED_PERMANENTLY :
			_extra_args["Location"] = _serv->_return_info.dir;
			_dir = DEFAULT_MOVED_FILE;
			SAY("Preparing 301 response. Redir: " << _serv->_return_info.dir << "\n");
		 break ;
		case FOUND :
			_extra_args["Location"] = _serv->_return_info.dir;
			_dir = DEFAULT_MOVED_FILE;
			SAY("Preparing 302 response. Redir: " << _serv->_return_info.dir << "\n");
		 break ;
		case PERMANENT_REDIRECT :
			SAY("Preparing 308 response. New URL: " << _serv->_return_info.dir << "\n");
			_dir = DEFAULT_MOVED_FILE;
			_body = REDIR_URL(_serv->_return_info.dir);
		 return 0;
		case TEMPORARY_REDIRECT :
			SAY("Preparing 307 response. New URL: " << _serv->_return_info.dir << "\n");
			_dir = DEFAULT_MOVED_FILE;
			_body = REDIR_URL(_serv->_return_info.dir);
		 return 0;
		case _REQUEST_DIR_LISTING :
			SAY("Autoindexing at \'" << _dir << "\'...\n");
			if (_dir[_dir.size() - 1] != '/')
				_dir.append("/");
			Webserv::getInstance()._cgi_man.start(_serv, _serv->getEnv(CGI_AUTOINDEX_DIR, getLoc()), _dir);
			if (Webserv::_up == -1)
				return internalServerError();
			_res_code = _DONT_SEND;
		 return -1;
		case NO_CONTENT :
			SAY("file deleted successfully" << std::endl);
		 return 0;
		case _POST_SUCCESS :
			SAY("Post request was successful. Posted at: " << _dir << "\n");
			_body = SUCCESSFUL_POST_PAGE;
			_res_code = OK;
		 return 0;
		case _CGI_RETURN :
			Webserv::getInstance()._cgi_man.start(_serv, getLoc()->stuff[LOC_CGI_RETURN], _dir, _serv->_query_str, _serv->getReqBody());
			if (Webserv::_up == -1)
				return internalServerError();
			_res_code = _DONT_SEND;
		 return -1;
		default:
			timestamp("Could not send file at path \'" + _dir + "\'. Res code: " + itoa(_res_code) + '\n', ERROR);
			return 0;
		}
		_body = Parsing::read_file(_dir) + '\n';
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		SAY("Body not constructed! ):\n");
		_body = DEFAULT_ERROR_STRING(_res_code);
	}
	return 0;
}

void Responser::addHeader()
{

	string		extra_head = _serv->getEnv("add_header", getLoc()), t;
	str_set_t	set;
	size_t		c;

	if (extra_head.empty())
		return ;
	if (extra_head != (t =_serv->getEnv("add_header")))
		extra_head.append(";" + t);

	while ((c = extra_head.find(";")) != string::npos)
	{
		set.insert(extra_head.substr(0, c));
		extra_head = extra_head.substr(c + 1);
	}
	set.insert(extra_head);
	for (str_set_t::iterator i = set.begin(); i != set.end(); ++i)
		_head.append(*i + CRNL);
}

void Responser::buildResponseHeader()
{
	time_t now = time(0);
	_head.reserve(HEAD_RESERVE);
	_head = "HTTP/1.1 " + itoa(_res_code) + ' ' + Webserv::getInstance().badExplain(_res_code) + CRNL;
	for (conf_t::iterator i = _extra_args.begin(); i != _extra_args.end(); ++i)
		_head.append(i->first + ": " + i->second + CRNL);
	if (_body.size())
	{
		_head.append("Content-Type: " + getDocType() + CRNL);
		_head.append("Content-Length: " + itoa(_body.size()) + CRNL);
	}
	addHeader();
	_head.append("Server: " + _serv->getEnv(NAME) + CRNL);
	_head.append("Date: " + string(ctime(&now)));
	_head.erase(_head.size() - 1,1);
	_head += DCRNL;
}

string	Responser::getDocType()
{
	size_t	dot = _dir.find_last_of('.');
	string	t;

	if (dot == string::npos)
		return "default";
	if ((t = Webserv::getInstance().findDocType(_dir.substr(dot))).empty())
		return "default";
	return t;
}

void	Responser::Send(int fd)
{
	long	t;
	
	SAY("Trying to send " << size() << " bytes to " << fd);
	if (_serv->getReqType() == HEAD)
		t = send(fd, _head.c_str(), _head.size(), MSG_EOR);
	else
		t = send(fd, (_head + _body).c_str(), size(), MSG_EOR);
	if (t < 0)
	{
		timestamp("send failed! ): Closing connection.\n", ERROR);
		_res_code = _DONT_SEND;
		keepalive = false;
		return ;
	}
	if (!t)
	{
		timestamp("Sent nothing! Assuming end of connection.\n", ERROR);
		_res_code = _DONT_SEND;
		keepalive = false;
		return ;
	}
	SAY("; Sent " << t << " bytes; body size was " <<  _body.size() << "... ");
	timestamp("Done!\n", DONE, BOLD, false);
}
