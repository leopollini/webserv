/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:08:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/02 18:53:15 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(port_t port) : _port(port), _down_count(0), _state(0), _clientfds()
{
	timestamp("Added server at port " + itoa(_port) + "!\n", CYAN);
	
}

Server::~Server()
{
	timestamp("Server at " + itoa(_port) + " removed!\n", BLUE);
	down();
	if (_sock.sock >= 0)
		close(_sock.sock);
	for (std::list<int>::iterator i = _clientfds.begin(); i != _clientfds.end(); i++)
		close(*i);
}

int	Server::Accept()
{
	int t = _sock.Accept();
	_clientfds.push_front(t);

	getsockname(_clientfds.front(), (sockaddr *)&_sock.client, &_sock.len);
	
	timestamp("Server at " + itoa(_port) + " caught a client! IP: " + addr_to_str(_sock.client.sin_addr.s_addr) + '\n', CONNECT);
	

	// fcntl(_clientfd, F_SETFL, fcntl(_clientfd, F_GETFL, 0) | O_NONBLOCK);
	
	// write(_clientfds.front(), "Hahalol\n", 8);
	
	// close(_clientfds.front());
	// _clientfds.pop_front();
	// close(_sock.sock);
	return _clientfds.front();
}

int	Server::getSockFd()
{
	return _sock.fd;
}

bool	Server::tryup()
{
	_state = 0;
	if (!_state)
		up();
	return 1;
}

void	Server::up()
{
	_sock.init(_port);
	timestamp("Port " + itoa(_port) + " now open!\n", CYAN);
	_state = 1;
}

void	Server::down()
{
	if (!_state)
		return ;
	_sock.down();
	timestamp("Port " + itoa(_port) + " now closed!\n", YELLOW);
	_state = 0;
}

req_t	Server::recieve(int fd)
{
	
	if (!(_msg_len = read(fd, _recieved_head, HEAD_BUFFER)))
		return FINISH;
	if (_msg_len == HEAD_BUFFER)
		throw HeadMsgTooLong();
	_recieved_head[_msg_len] = 0;
	return parseMsg(fd);
}

void	Server::respond(int fd)
{
	std::cout << "Called server at port " << _port << ", responding...!\n";
}

void	Server::closeConnection(int fd)
{
	timestamp("Closing fd: " + itoa(fd) + "!\n", INFO);
	close(fd);
}

void	Server::printHttpRequest(string &msg, int fd_from)
{
	timestamp("Recieved from connection at fd " + itoa(fd_from) + ": " + msg + "\n", REC_MSG_PRNT);
}

req_t	Server::parseMsg(int fd)
{
	string	msg(_recieved_head);
	string	cmd;
	size_t	space_pos;

	msg = msg.substr(0, msg.find('\n'));
	printHttpRequest(msg, fd);
	cmd = msg.substr(0, space_pos = msg.find(' '));
	
	if (space_pos > msg.size())
		return INVALID;

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

	std::cout << "requested dir be: \'" << _current_request.dir << "\'\n";
	return _current_request.type;
}
