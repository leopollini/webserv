/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server_utils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:08:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/23 19:08:01 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Responser.hpp"
#include "Webserv.hpp"
#include <poll.h>

Server::Server(short id) : _resp(this), _clientfds(), _id(id), _state(0), _current_request(), _lastUpAttempt(0), _down_count(0)
{
	// _received_head[BUFFER_SIZE] = 0;
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
	_current_request = request_t();
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

//tries to raise the server (opening its socket) after retry_time (SERVER_RETRY_TIME by default) 
bool Server::tryUp(time_t retry_time)
{
	_state = 0;
	time_t now = time(NULL);
	if (now - _lastUpAttempt < retry_time) //wait more to retry
		return (false);
	// _lastUpAttempt;
	if (!_state)
		up();
	return (true);
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

/*// splits line at the first \n then header from body at "\r\n\r\n"
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
}*/

/*static string read_remaining_body(char *read_buffer, size_t to_read, int fd)
{
	string	body_tail = "";
	pollfd	ps = {fd, POLLIN, 0};
	int		bytes_read = 1;
	
	while (to_read && bytes_read > 0)
	{
		// if (poll(&ps, 1, 1) < 1 || !(ps.revents & POLLIN))
			// return (body_tail);
		
		bytes_read = read(fd, read_buffer, std::min(to_read, static_cast<size_t>(BUFFER_SIZE)));
		if (bytes_read < 0)
			break;
			
		read_buffer[bytes_read] = 0;
		body_tail += read_buffer;
		to_read -= bytes_read;
	}
	return (body_tail);
}*/


/*static void parse_request_header(string header, request_t &request)
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

}*/

/*static req_t receive_body(request_t request) throw (Server::BodyMsgTooLong)
{

	bool chunked_encoding = transfer_encoding::is_encoded(request);

	size_t body_size = static_cast<size_t>(atoi(request.header[H_BODY_SIZE].c_str()));
	string &max_body_size = getEnv(L_MAX_BODY_SIZE, request.loc);

	if (max_body_size != "" && body_size > (size_t)atoi(max_body_size.c_str()))
		throw Server::BodyMsgTooLong();
	

	if ( bytes_read == BUFFER_SIZE && (chunked_encoding || body_size > request.body.size()) )
	{
		request.complete = false;
		return INCOMPLETE;
	}
	transfer_encoding::decode_request_body(request);
}*/

/*req_t Server::receive(int fd)
{
	request_t	&request = _current_request;
	string		req_line;
	string		req_header;
	size_t		bytes_read;
	char		*read_buffer;
	cout << "Readimg from " << fd << "...\n";
	
	if (!request.complete)
		return (continue_incomplete_request(request, fd)); //can also return incomplete
	
	
	
	if (!bytes_read)
		return FINISH;

	req_line = string(read_buffer, bytes_read);
	size_t head_end = req_line.find("\r\n\r\n");
	if (bytes_read == BUFFER_SIZE && head_end == string::npos)
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
		receive_body(request);
	if (request.type != INCOMPLETE)
		printHttpRequest(request);
	// return parseMsg(fd);
	return request.type;
}*/



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
