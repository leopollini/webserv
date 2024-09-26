/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:08:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/26 17:57:47 by lpollini         ###   ########.fr       */
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
	_env[PORT] = SERVER_DEFAULT_PORT;
	_env[LOC_ROOT] = SERVER_DEFAULT_ROOT;
	_env[NAME] = SERVER_DEFAULT_NAME;
}

Server::~Server()
{
	timestamp("Server " + itoa(_id) + " removed!\n", BLUE);
	down();
	if (_sock.sock >= 0)
		close(_sock.sock);
	for (fd_list_t::iterator i = _clientfds.begin(); i != _clientfds.end(); i++)
		close(*i);
	for (locations_list_t::iterator i = _loc_ls.begin(); i != _loc_ls.end(); i++)
		delete *i;
	close(_sock.fd);
}

int	Server::Accept()
{
	int	t = _sock.Accept();

	_clientfds.push_front(t);
	_current_request = request_t();
	getsockname(_clientfds.front(), (sockaddr *)&_sock.client, &_sock.len);
	SAY("New client at IP: " << addr_to_str(_sock.client.sin_addr.s_addr) << "!\n");
	fcntl(_clientfds.front(), F_SETFL, fcntl(_clientfds.front(), F_GETFL, 0) | O_NONBLOCK);
	return _clientfds.front();
}

void	Server::addLocation(location_t *l)
{
	l->dir = l->stuff[L_DIR];
	if (l->dir.empty())
		throw EmptyLocationDir();
	l->allows = read_allows(l->stuff[L_ALLOW_METHODS]);

	// is needed???
	for (locations_list_t::iterator i = _loc_ls.begin(); i != _loc_ls.end(); i++)
		if ((*i)->dir == l->dir)
			throw DuplicateServLocation();
	_loc_ls.push_back(l);
}

// Anything regarding initialization of env (both server's and locations') MUST be done here
void	Server::setup()
{
	for (locations_list_t::iterator i = _loc_ls.begin(); i != _loc_ls.end(); i++)
	{
		string	&line = (*i)->dir;
		if (line.find(' ') != string::npos)
		{
			(*i)->allowed_extensions.insert(line.substr(line.find(' ') + 1));
			line.erase(line.find(' '));
		}
		(*i)->stuff[L_DIR] = line;
	}
}

//tries to raise the server (opening its socket) after retry_time (SERVER_RETRY_TIME by default) 
bool Server::tryUp(time_t retry_time)
{
	_state = 0;
	time_t now = time(NULL);
	if (_is_sharing_port == -1 || now - _lastUpAttempt < retry_time) //wait more to retry
		return (false);
	// _lastUpAttempt;
	if (!_state)
		up();
	return (_state);
}

void Server::up()
{
	if (BetterSelect::_used_ports.count(getPort()))
	{
		Server	*t = BetterSelect::_used_ports[getPort()].front();
		t->_is_sharing_port = 1;
		_is_sharing_port = -1;
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
	
	index_file = getEnv(L_INDEX, _current_request.loc);
	if (index_file.empty())									// no index file found
		return FORBIDDEN;

	if (_resp.getDir()[_resp.getDir().size() - 1] != '/')
		_resp.getDir() += '/';
		
	index_file = _resp.getDir().append(index_file); //searches for index files
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



static void add_meta_variables(Server *s, string query_string, string body, BetterEnv &env)
{
	if (!query_string.empty())
		env.addVariable(QUERY_STR_ENV, query_string);
	if (body.size())
		env.addVariable(CONTENT_SIZE, itoa(body.size()));
	

}

static void location_fuckery(string &cgi_dir, string &uri_dir, Server *s)
{
	size_t first_slash = uri_dir.find('/');

	while (first_slash != string::npos)
	{
		cgi_dir = cgi_dir
	}
}

// CGI function!!!
void	CGIManager::start(Server *s, const string &cgi_dir, const string &uri_dir, string query_string, string body)
{
	BetterEnv					env(_envp);
	std::vector<const char *>	args;
	pid_t						fk;
	int							t[2];
	int							pipefd[2];

	SAY("Trying to execute " << args[0] << '\n');

	pipe(pipefd);
	if (!(fk = fork()))
	{
		add_meta_variables(s, query_string, body, env);

		location_fuckery(cgi_dir, uri_dir, s);
		args.push_back(cgi_dir.c_str());
		args.push_back(uri_dir.c_str());
		args.push_back(NULL);


		close(pipefd[1]);
		t[1] = dup(STDOUT_FILENO);
		t[0] = dup(STDIN_FILENO);
		dup2(s->getFd(), STDOUT_FILENO);
		dup2(pipefd[0], STDIN_FILENO);
		execve(args[0], (char *const*)args.data(), env.c_envp());
		dup2(t[1], STDOUT_FILENO);
		dup2(t[0], STDIN_FILENO);

		close(STDOUT_FILENO);
		close(pipefd[0]);
		close(STDIN_FILENO);
		close(t[0]);
		close(t[1]);
		
		Webserv::_up = -1;
		std::cerr << "A CGI crashed!!!\n";
		return ;
	}
	close(pipefd[0]);
	if (!body.empty())
		write(pipefd[1], body.c_str(), body.size());
	close(pipefd[1]);

	_pids.push_back(fk);
	timestamp("CGI started!!\n");
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
	_is_returning = false;
}

char	Responser::internalServerError()
{
	SAY("Preparing 500 response.\n");
	_dir = _serv->getEnv(E_500, _loc);
	_body = Parsing::read_file(_dir);
	_res_code = INTERNAL_SEVER_ERROR;
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
			SAY("Reading \n");
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
			Webserv::getInstance()._cgi_man.start(_serv, _serv->getEnv(CGI_AUTOINDEX_DIR, getLoc()), _dir);
			if (Webserv::_up == -1)
				return internalServerError();
			_res_code = _DONT_SEND;
		 return -1;
		case _REQUEST_DELETE :
			Webserv::getInstance()._cgi_man.start(_serv, _serv->getEnv(CGI_DELETE_DIR, getLoc()), _dir);
			if (Webserv::_up == -1)
				return internalServerError();
			_res_code = _DONT_SEND;
		 return -1;
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
		_body = Parsing::read_file(_dir);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		SAY("Body not constructed! ):\n");
	}
	return 0;
}

void Responser::buildResponseHeader()
{
	time_t now = time(0);
	_head.reserve(HEAD_RESERVE);
	_head = "HTTP/1.1 " + itoa(_res_code) + ' ' + Webserv::getInstance().badExplain(_res_code) + CRNL;
	for (conf_t::iterator i = _extra_args.begin(); i != _extra_args.end(); ++i)
		_head.append(i->first + ": " + i->second + CRNL);
	_head.append("Content-Type: " + getDocType() + CRNL);
	_head.append("Content-Length: " + itoa(_body.size()) + CRNL);
	_head.append("Server: " + _serv->getEnv(NAME) + CRNL);
	_head.append("Date: " + string(ctime(&now)));
	_head.erase(_head.size() - 1,1); //removes unwanted trailing /n from ctime
// _head.append(string("Keepalive: false") + CRNL);
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
		return (void)timestamp("send failed! ):\n", ERROR);
	SAY("; Sent " << t << " bytes; body size was " <<  _body.size() << "... ");
	timestamp("Done!\n", DONE, BOLD, false);
}
