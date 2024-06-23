/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:08:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/23 02:03:02 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Responser.hpp"
#include "Webserv.hpp"
#include <poll.h>

Server::Server(short id) : _id(id), _clientfds(), _state(0), _down_count(0), _resp(this), _current_request()
{
	// _received_head[HEAD_BUFFER] = 0;
	timestamp("Added new Server! Id: " + itoa(_id) + "!\n", CYAN);
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

void Server::HttpRequestLog(string &request_line, int fd_from)
{
	timestamp("Server " + itoa(_id) + ": Recieved from connection at fd " + itoa(fd_from) + ":\n\t" + request_line + "\n", REC_MSG_PRNT);
}

status_code_t	Server::manageDir()
{
	string 	index_file;

	if (_resp.getLoc()->stuff[L_AUTOINDEX] == "yes")		// autoindex on
		return (_REQUEST_DIR_LISTING);
	
	index_file = getEnv(L_INDEX, _current_request.loc);
	if (index_file.empty())									// no index file found
		return (FORBIDDEN);

	if (_resp.getDir()[_resp.getDir().size() - 1] != '/')
		_resp.getDir() += '/';
		
	index_file = _resp.getDir().append(index_file); //searches for index files
	char	index_flags = checkCharacteristics(index_file.c_str());

printf("called. %s\n", index_file.c_str());
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

//sets method type, url and protocol version (GET, /, HTTP/1.1)
static void parse_request_line(request_t &request, const string &req_line)
{	
	size_t	field_begin;
	if (!req_line.compare(0, 4, "GET "))
	{
		request.type = GET;
		field_begin = 4;
	}
	else if (!req_line.compare(0, 5, "POST "))
	{
		request.type = POST;
		field_begin = 5;
	}
	else if (!req_line.compare(0, 7, "DELETE "))
	{
		request.type = DELETE;
		field_begin = 7;
	}
	else if (!req_line.compare(0, 5, "HEAD "))
	{
		request.type = HEAD;
		field_begin = 5;
	}
	else
	{
		request.type = INVALID;
		return ;
	}

	field_begin = req_line.find_first_not_of(' ', field_begin);
	if (field_begin == string::npos)
	{
		request.type = INVALID;
		return ;
	}

	size_t	field_end = req_line.find(' ', field_begin);
	size_t	protocol_version = req_line.find("HTTP/");
	if (field_end == string::npos || protocol_version == string::npos)
	{
		if (field_end != string::npos)
		{
			request.type = INVALID;
			return ;
		}
		string(DEFAULT_PROTOCOL).copy(request.http_version, 3, 0);
		request.dir = req_line.substr(field_begin);
		return ;
	}
	request.dir = req_line.substr(field_begin, field_end - field_begin);
	
	req_line.copy(request.http_version, 3, protocol_version + 5);
	//checks if http_version matches "1.n" or "1"
	if (request.http_version[0] != '1' ||
	(request.http_version[1] != '\0' && (request.http_version[1] != '.' || !isdigit(request.http_version[2]))))
		request.type = INVALID;
}	

// splits line at the first \n then header from body at "\r\n\r\n"
static void	split_request(string &line, string &header, string &body)
{
	header = "";
	body = "";
	size_t line_start = line.find_first_not_of(' ');
	if (line_start == string::npos)
		return ;

	size_t header_start = line.find("\r\n");
	if (header_start == string::npos)
		return ;

	size_t body_start = line.find("\r\n\r\n", header_start);
	if (body_start != string::npos)
		body = line.substr(body_start + 4);
	else
		body_start = line.size();
	header = line.substr(header_start + 2, body_start - header_start);
	line = line.substr(line_start, header_start - line_start);
}

static string read_remaining_body(char *read_buffer, size_t to_read, int fd)
{
	string	body_tail = "";
	pollfd	ps = {fd, POLLIN, 0};
	int		bytes_read = 1;
	
	while (to_read && bytes_read > 0)
	{
		if (poll(&ps, 1, 1) || !(ps.revents & POLLIN))
			return (body_tail);
		
		bytes_read = read(fd, read_buffer, std::min(to_read, static_cast<size_t>(HEAD_BUFFER)));
		if (bytes_read < 0)
			break;
			
		read_buffer[bytes_read] = 0;
		body_tail += read_buffer;
		to_read -= bytes_read;
	}
	return (body_tail);
}

static void parse_request_header(string header, request_t &request)
{
	//parses header into request variable map (request.header)
	std::map<string, string> &map = request.header;
	
	map.clear();
	for (size_t i = 0, endl; i < header.size(); i = endl + 1)
	{
		endl = header.find("\r\n", i);
		if (endl == string::npos)
			endl = header.size();
		
		string key, value;
		size_t	separator = header.find(':', i);
		if (separator == string::npos)
			continue;
		key = strip(header.substr(i, separator - i), " \n\r");
		value = strip(header.substr(separator + 1, endl - separator), " \n\r");
		map[key] = value;
	}

}

req_t Server::receive(int fd)
{
	request_t	&request = _current_request;
	string		req_line;
	string		req_header;
	size_t		bytes_read;
	char		read_buffer[HEAD_BUFFER + 1];
	read_buffer[HEAD_BUFFER] = 0;
	cout << "Readimg from " << fd << "...\n";
	
	bytes_read = read(fd, read_buffer, HEAD_BUFFER); // this also reads the body. What if the body exceeds the head_buffer size?
	
	if (!bytes_read)
		return FINISH;

	req_line = string(read_buffer, bytes_read);
	size_t head_end = req_line.find("\r\n\r\n");
	if (bytes_read == HEAD_BUFFER && head_end == string::npos)
		throw HeadMsgTooLong(); // html error 431 (request header too large)
	
	split_request(req_line, req_header, request.body);
	HttpRequestLog(req_line, fd);
	parse_request_line(request, req_line);
	if (request.type == INVALID)
		return (INVALID);
	
	parse_request_header(req_header, request);
	// truncate location identification part of dir
	matchRequestLocation(request);
	request.littel_parse(this); //wtf does this even do

	if (request.type == POST || request.type == DELETE) // all handled methods with body
	{
		size_t body_size = static_cast<size_t>(atoi(request.header.at(H_BODY_SIZE).c_str()));
		string &max_body_size = getEnv(L_MAX_BODY_SIZE, request.loc);

		if (max_body_size != "" && body_size > atoi(max_body_size.c_str()))
			throw BodyMsgTooLong();

		if (bytes_read == HEAD_BUFFER && body_size > request.body.size())
			request.body += read_remaining_body(read_buffer, body_size - request.body.size(), fd);
	}
	// printHttpRequest(request, std::cout);
	// return parseMsg(fd);
	return request.type;
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

void Responser::buildResponseHeader()
{
	time_t now = time(0);
	_head.reserve(HEAD_RESERVE);
	_head = "HTTP/1.1 " + itoa(_res_code) + ' ' + badExplain(_res_code) + CRNL;
	_head.append("Content-Type: " + getDocType() + CRNL);
	_head.append("Content-Length: " + itoa(getBodyLen()) + CRNL);
	_head.append("Server: " + _serv->getEnv(NAME) + CRNL);
	_head.append("Date: " + string(ctime(&now)));
	if (_res_code == MOVED_PERMANENTLY)						// this happens during redirection
		_head.append("Location: " + _dir);
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
