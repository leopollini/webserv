/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:08:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/08 20:13:29 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Responser.hpp"
#include "Webserv.hpp"

Server::Server(short id) : _id(id), _clientfds(), _state(0), _down_count(0), _resp(this)
{
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

int Server::Accept()
{
	int t = _sock.Accept();
	_clientfds.push_front(t);

	getsockname(_clientfds.front(), (sockaddr *)&_sock.client, &_sock.len);

	timestamp("Server " + itoa(_id) + " caught a client! IP: " + addr_to_str(_sock.client.sin_addr.s_addr) + '\n', CONNECT);

	fcntl(_clientfds.front(), F_SETFL, fcntl(_clientfds.front(), F_GETFL, 0) | O_NONBLOCK);

	return _clientfds.front();
}

void	Server::addLocation(location_t *l)
{
	l->dir = l->stuff[L_DIR];
	for (locations_list::iterator i = _loc_ls.begin(); i != _loc_ls.end(); i++)
		if ((*i)->dir == l->dir)
		{
			printf("called. %s\n", l->dir.c_str());
			throw DuplicateServLocation();
		}
	_loc_ls.push_back(l);
}

int Server::getSockFd()
{
	return _sock.fd;
}

bool Server::tryup()
{
	_state = 0;
	if (!_state)
		up();
	return 1;
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

req_t Server::recieve(int fd)
{
	cout << "Readimg from " << fd << "...\n";
	if (!(_msg_len = read(fd, _recieved_head, HEAD_BUFFER)))
		return FINISH;
	if (_msg_len == HEAD_BUFFER)
		throw HeadMsgTooLong();
	_recieved_head[_msg_len] = 0;
	return parseMsg(fd);
}

req_t Server::parseMsg(int fd)
{
	string msg(_recieved_head);
	string cmd;
	size_t space_pos;

	msg = msg.substr(0, msg.find('\n'));
	printHttpRequest(msg, fd);
	cmd = msg.substr(0, space_pos = msg.find(' '));

	if (space_pos > msg.size())
		return INVALID;
	//  FIND HOST!!!
	_current_request.dir = msg.substr(space_pos + 1, msg.find(' ', space_pos + 1) - space_pos - 1);
	_current_request.type = INVALID;
	if (cmd == "GET")
		_current_request.type = GET;
	else if (cmd == "POST")
		_current_request.type = POST;
	else if (cmd == "DELETE")
		_current_request.type = DELETE;
	else if (cmd == "HEAD")
		_current_request.type = HEAD;

	matchRequestLocation(_current_request);
	// truncate location identification part of dir
	if (_current_request.loc)
		_current_request.dir = _current_request.dir.substr(_current_request.loc->dir.size());
	else
		cout << "LOCATION NOT FOUND!\n";
	if (_current_request.dir[0] != '/')
		_current_request.dir = '/' + _current_request.dir;
	_current_request.dir = getEnv(LOC_ROOT, _current_request.loc) + _current_request.dir;
	std::cout << "requested dir be: \'" << _current_request.dir << "\'\n";
	return _current_request.type;
}

//matches the request directory with a location and sets its location_t pointer
void Server::matchRequestLocation(request_t &request) const
{
	if (request.type == INVALID)
		// *(int *)(0); //crash
		return ;

	size_t max_len = 0;
	location_t *location = NULL;
	
	for (locations_list::const_iterator it = _loc_ls.begin(); it != _loc_ls.end(); it++)
	{
		string	&dir = (*it)->dir;

		// printf("called. \'%s\' (%i)\n", dir.c_str(), dir.size());
		//if directory is more specific, or if it doesn't match
		if (dir.size() - 1 > request.dir.size() || dir.size() <= max_len)
			continue ;

		if (request.dir.find(dir) == string::npos)
			continue ;

		int i = 0;
		for (; dir[i]; i++)
			if (dir[i] != request.dir[i])
				break ;
		if (dir[i - 1] != '/' && (dir[i] || (request.dir[i] != '/' && request.dir[i])))
			continue ;

		location = *it;
		max_len = dir.size();
	}
	request.loc = location;
	if (location)
	{
		cout << "Found location: " << location->dir << '\n';
		cout << "location's root: " << location->stuff[LOC_ROOT] << '\n';
	}
}

status_code_t	Server::validateLocation()
{
	// loc->allows not initialized yet
	// if (!(request.loc->allows & _current_request.type))
	// 	return (METHOD_NOT_ALLOWED);
	

	// printf("called. %p'\n", request.loc);
	// check '//'

	cout << "Looking for " << _current_request.dir << "\n";
	
	_resp = _current_request;
	
	string target_file;
	char flags = checkCharacteristics(_current_request.dir.c_str());

	if (!C_OK(flags))
		return (NOT_FOUND);
	if (flags & C_DIR)// is a directory
	{
		target_file = getEnv(L_INDEX, _current_request.loc); //searches for index files
		/*
		DIR LISTING ON:
			look for index (default be index.html) in
				location,
				server ,
				html
			list dirs e basta
		DIR LISTIN OFF:
			look for index (default be index.html) in
				location,
				server ,
				html
			404
			
		*/
		if (_current_request.dir.empty())
			return (NOT_FOUND);
	}

	return (OK);
}

// If nothing is found, returns ""
string	Server::getEnv(string key, location_t *location) const
{
	string value;
	conf_t::const_iterator var;

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



void Server::respond(int fd)
{
	std::cout << "Server " + itoa(_id) + ": Called by fd " << fd << " for response!\n";

	// FIND correct location
	// Set Responser's location
	// Launch Responser buildBody and buildHeader
	// Send response

	_resp._res_code = validateLocation() << '\n';
	_resp.buildResponseBody();
	_resp.buildResponseHeader();
	_resp.Send(fd);
	_resp.clear();
}

void Responser::buildResponseBody()
{
	_res_code = OK;
	//check file existance
	_body = Parsing::read_file(_dir);
}

void Responser::buildResponseHeader()
{
	time_t now = time(0);
	_head = "HTTP/1.1 " + itoa(_res_code) + ' ' + badExplain(_res_code) + CRNL;
	_head.append("Content-Type: " + getDocType() + CRNL);
	_head.append("Content-Length: " + itoa(getBodyLen()) + CRNL);
	_head.append("Server: " + _serv->serverGetEnv(NAME) + CRNL);
	_head.append("Date: " + string(ctime(&now)));
	_head += CRNL;
}
