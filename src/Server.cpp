/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:08:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/06 15:55:31 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

// //Deprecated. Don't use
// Server::Server(port_t port) : _clientfds(), _state(0), _down_count(0) 
// {
// 	_env[NAME] = "default";
// 	_env[PORT] = itoa(port);
// 	timestamp("Added server at port " + _env[PORT] + ", named " + _env[NAME] + "!\n", CYAN);
// }

//Deprecated. Don't use
Server::Server(port_t port, string name) : _clientfds(), _state(0), _down_count(0)
{
	_env[NAME] = name;
	_env[PORT] = itoa(port);
	timestamp("Added server at port " + _env[PORT] + ", named " + _env[NAME] + "!\n", CYAN);
}

Server::Server(short id) : _id(id), _clientfds(), _state(0), _down_count(0)
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

int	Server::Accept()
{
	int t = _sock.Accept();
	_clientfds.push_front(t);

	getsockname(_clientfds.front(), (sockaddr *)&_sock.client, &_sock.len);
	
	timestamp("Server " + itoa(_id) + " caught a client! IP: " + addr_to_str(_sock.client.sin_addr.s_addr) + '\n', CONNECT);

	fcntl(_clientfds.front(), F_SETFL, fcntl(_clientfds.front(), F_GETFL, 0) | O_NONBLOCK);

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
	_sock.init(atoi(_env[PORT].c_str()));
	timestamp("Server " + itoa(_id) + ": Port " + _env[PORT] + " now open!\n", CYAN);
	_state = 1;
}

void	Server::down()
{
	if (!_state)
		return ;
	_sock.down();
	timestamp("Server " + itoa(_id) + ": Port " + _env[PORT] + " now closed!\n", YELLOW);
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

void	Server::closeConnection(int fd)
{
	timestamp("Server " + itoa(_id) + ": Closing fd: " + itoa(fd) + "!\n", INFO);
	close(fd);
}

void	Server::printHttpRequest(string &msg, int fd_from)
{
	timestamp("Server " + itoa(_id) + ": Recieved from connection at fd " + itoa(fd_from) + ":\n\t" + msg + "\n", REC_MSG_PRNT);
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

	std::cout << "requested dir be: \'" << _current_request.dir << "\'\n";
	return _current_request.type;
}

void	Server::respond(int fd)
{
	std::cout << "Server " + itoa(_id) + ": Called fd " << fd << " for response!\n";

	_res_code = 200;


	// int file = open("file.html", O_RDONLY);
	// char	asd[200];
	// int	readed;
	// do
	// {
	// readed = read(file, asd, 199);
	// asd[readed] = 0;
	// for (int i = 0; asd[i]; i++)
	// 	write(fd, asd + i, 1);
	// } while (readed == 199);
	// write(fd, "\r\n", 2);

	_current_response.head = "HTTP/1.1 " + itoa(_res_code) + ' ' + badExplain(_res_code) + CRNL;
	buildResponseHeader();
	send(fd, (_current_response.head + _current_response.body).c_str(), (_current_response.Size()), MSG_EOR);
	_current_response.Clear();
}

void	Server::buildResponseHeader()
{
	time_t now = time(0);
	_current_response.head.append(ctime(&now));
	_current_response.head += '\r';
	_current_response.head.append("Context-Type: " + getDocType() + CRNL);
	_current_response.head.append("Context-Length: " + itoa(getResLen()) + CRNL);
	_current_response.head.append("Server: " + _env[NAME] + CRNL);
}
