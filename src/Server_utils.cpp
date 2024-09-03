/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:08:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/03 18:39:26 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Responser.hpp"
#include "Webserv.hpp"


Server::Server(short id) : _clientfds(), _id(id), _state(0), _resp(this), _down_count(0)
{
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
	for (fd_list::iterator i = _clientfds.begin(); i != _clientfds.end(); i++)
		close(*i);
	for (locations_list::iterator i = _loc_ls.begin(); i != _loc_ls.end(); i++)
		delete *i;
	close(_sock.fd);
}

int	Server::Accept()
{
	int	t = _sock.Accept();

	_clientfds.push_front(t);
	getsockname(_clientfds.front(), (sockaddr *)&_sock.client, &_sock.len);
	timestamp("Server " + itoa(_id) + " caught a client! IP: " + addr_to_str(_sock.client.sin_addr.s_addr) + '\n', CONNECT);
	fcntl(_clientfds.front(), F_SETFL, fcntl(_clientfds.front(), F_GETFL, 0) | O_NONBLOCK);
	return _clientfds.front();
}

void	Server::addLocation(location_t *l)
{
	l->dir = l->stuff[L_DIR];
	if (l->dir.empty())
		throw EmptyLocationDir();
	l->allows = read_allows(l->stuff[L_ALLOW_METHODS]);
	for (locations_list::iterator i = _loc_ls.begin(); i != _loc_ls.end(); i++)
		if ((*i)->dir == l->dir)
		{
			printf("called. %s\n", l->dir.c_str());
			throw DuplicateServLocation();
		}
	_loc_ls.push_back(l);
}

// Anything regarding initialization of env (both server's and locations') MUST be done here
void	Server::setup()
{
	for (locations_list::iterator i = _loc_ls.begin(); i != _loc_ls.end(); i++)
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

void Server::tryup()
{
	_state = 0;
	if (!_state)
		up();
	return ;
}

void Server::up()
{
	_sock.init(atoi(_env[PORT].c_str()));

	timestamp("Server " + itoa(_id) + ": Port " + _env[PORT] + " now open!\n", CYAN);
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
	timestamp("Server " + itoa(_id) + ": Recieved from connection at fd " + itoa(fd_from) + ":\n\t" + msg + "\n", REC_MSG_PRNT);
}

status_code_t	Server::manageDir()
{
	string 	index_file;

	if (getEnv(L_AUTOINDEX, _resp.getLoc()) == "yes")		// autoindex on
		return (_REQUEST_DIR_LISTING);
	
	index_file = getEnv(L_INDEX, _current_request.loc);
	if (index_file.empty())									// no index file found
		return (FORBIDDEN);

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
		return (FORBIDDEN);
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
		dir = dir.substr(loc->dir.size());
	else
		cout << "LOCATION NOT FOUND!\n";
	if (dir[0] != '/')
		dir = '/' + dir;
	dir = s->getEnv(LOC_ROOT, loc) + dir;
	while (*--dir.end() == '/')	// erases trailing '/'s if present
		dir.erase(--dir.end());
	std::cout << "requested dir be: \'" << dir << "\'\n";
}

void	CGIManager::start(Server *s, const string cgi_path, ...)
{
	va_list						ap;
	std::vector<const char *>	args;
	char const 					*temp;
	pid_t						fk;

	va_start(ap, cgi_path);
	args.push_back(cgi_path.c_str());
	do {
		temp = va_arg(ap, const char *);
		args.push_back(temp);
	} while (temp);
	
	if (!(fk = fork()))
	{
		dup2(s->getFd(), STDOUT_FILENO);
		execve(args[0], (char *const*)args.data(), _env);
		Webserv::getInstance().stop();
		// This child fails. Check again
		return ;
	}
	timestamp("Child started!!\n");
	
	va_end(ap);
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
			cout << "Reading \n";
		 break ;
		case NOT_FOUND :
			_dir = _serv->getEnv(E_404, _loc);
			cout << "Looking for 404 response" <<  "'\n";
		 break ;
		case FORBIDDEN :
			_dir = _serv->getEnv(E_403, _loc);
			cout << "Looking for 403 response in \n";
		 break ;
		case METHOD_NOT_ALLOWED :
			_dir = _serv->getEnv(E_405, _loc);
			cout << "Looking for 405 response in \n";
		 break ;
		case MOVED_PERMANENTLY :
			_extra_args["Location"] = _serv->_return_info.dir;
			_dir = DEFAULT_MOVED_FILE;
			cout << "Preparing 301 response. Redir: " << _serv->_return_info.dir << "\n";
		 break ;
		case FOUND :
			_extra_args["Location"] = _serv->_return_info.dir;
			_dir = DEFAULT_MOVED_FILE;
			cout << "Preparing 302 response. Redir: " << _serv->_return_info.dir << "\n";
		 break ;
		case PERMANENT_REDIRECT :
			cout << "Preparing 308 response. New URL: " << _serv->_return_info.dir << "\n";
			_dir = DEFAULT_MOVED_FILE;
			_body = REDIR_URL(_serv->_return_info.dir);
		 return 0;
		case TEMPORARY_REDIRECT :
			cout << "Preparing 307 response. New URL: " << _serv->_return_info.dir << "\n";
			_dir = DEFAULT_MOVED_FILE;
			_body = REDIR_URL(_serv->_return_info.dir);
		 return 0;
		case _REQUEST_DIR_LISTING :
			Webserv::getInstance()._cgi_man.start(_serv, AUTOINDEX_CGI_DIR, _dir.c_str(), "AJSHDGASDG", 0);
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
		cout << "Body not constructed! ):\n";
	}
	return 0;
}

void Responser::buildResponseHeader()
{
	time_t now = time(0);
	_head.reserve(HEAD_RESERVE);
	_head = "HTTP/1.1 " + itoa(_res_code) + ' ' + badExplain(_res_code) + CRNL;
	for (conf_t::iterator i = _extra_args.begin(); i != _extra_args.end(); ++i)
		_head.append(i->first + ": " + i->second + CRNL);
	_head.append("Content-Type: " + getDocType() + CRNL);
	_head.append("Content-Length: " + itoa(_body.size()) + CRNL);
	_head.append("Server: " + _serv->getEnv(NAME) + CRNL);
	_head.append("Date: " + string(ctime(&now)) + CRNL);
// _head.append(string("Keepalive: false") + CRNL);
	_head += CRNL;
}

string	Responser::getDocType()
{
	size_t	dot = _dir.find_last_of('.');

	if (dot == string::npos)
		return "default";
	string t = Webserv::getInstance().findDocType(_dir.substr(dot));
	if (t.empty())
		return "default";
	return t;
}

