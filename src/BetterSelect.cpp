/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterSelect.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:57:55 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/01 14:35:47 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/BetterSelect.hpp"

port_servs_map_t	BetterSelect::_used_ports;

BetterSelect::BetterSelect() : _tot_size(0)
{
	FD_ZERO(&_read_pool);
	FD_ZERO(&_write_pool);
	// FD_SET(STDIN_FILENO, &_read_pool);
}

BetterSelect::~BetterSelect()
{
	FD_ZERO(&_read_pool);
	FD_ZERO(&_write_pool);
	for (connections_map_t::iterator i = _clis_map.begin(); i != _clis_map.end(); i++)
		close(i->first);
	for (connections_map_t::iterator i = _servs_map.begin(); i != _servs_map.end(); i++)
		close(i->first);
}

void	BetterSelect::loadServFds(serv_list_t &ls)
{
	_tot_size = ls.size();
	_servs_map.clear();
	for (serv_list_t::iterator s = ls.begin(); s != ls.end(); s++)
		if ((*s)->getState() == 1)
			addConnectionServ((*s)->getSockFd(), *s);
}

void	BetterSelect::addListeningConnection(int fd, Server *s)
{
	FD_SET(fd, &_read_pool);
	_clis_map[fd] = s;
	_tot_size++;
	_timeout_map[fd] = time(NULL);
}

void	BetterSelect::delListeningConnection(int fd)
{
	FD_CLR(fd, &_read_pool);
	if (_clis_map[fd])
		_tot_size--;
	_clis_map[fd] = NULL;
}

void	BetterSelect::addResponseConnection(int fd, Server *s)
{
	FD_SET(fd, &_write_pool);
	_clis_map[fd] = s;
	_tot_size++;
}

void	BetterSelect::delResponseConnection(int fd)
{
	FD_CLR(fd, &_write_pool);
	if (_clis_map[fd])
		_tot_size--;
	_clis_map[fd] = NULL;
}

void	BetterSelect::addConnectionServ(int fd, Server *s)
{
	FD_SET(fd, &_read_pool);
	_servs_map[fd] = s;
	_tot_size++;
}

void	BetterSelect::delConnectionServ(int fd)
{
	FD_CLR(fd, &_read_pool);
	if (_servs_map[fd])
		_tot_size--;
	_servs_map[fd] = NULL;
}

void	BetterSelect::closeAllClis()
{
	for (connections_map_t::iterator i = _clis_map.begin(); i != _clis_map.end(); ++i)
		if (i->second)
			i->second->closeConnection(i->first);
}

int	BetterSelect::getBiggestFd()
{
	if (!_clis_map.size())
		return (--_servs_map.end())->first;
	return std::max((--_servs_map.end())->first, (--_clis_map.end())->first);
}

//iterates through connection map and if anyone has outlived its expiration time it's closed
void	BetterSelect::closeTimedOut()
{
	if (!_clis_map.size())
		return ;
	for (connections_map_t::iterator i = _clis_map.begin(); i != _clis_map.end(); ++i)
		if (i->second && time(NULL) - _timeout_map[i->first] > CONNECTION_TIMEOUT)
			rmFd(i->first, i->second);
	return ;
}

void	BetterSelect::_acceptNewConnections(fd_set &read_fds)
{
	for (connections_map_t::iterator i = _servs_map.begin(); i != _servs_map.end(); i++)
	{
		if (FD_ISSET(i->first, &read_fds))
		{
			int fd = i->second->Accept();
			addListeningConnection(fd, i->second);
			timestamp("Server " + itoa(i->second->getId()) + " created new connection at fd " + itoa(fd) + '\n', INFO);
		}
	}
}


void	BetterSelect::rmFd(int fd, Server *s)
{
	if (s)
		s->closeConnection(fd);
	delListeningConnection(fd);
}

static void	log_request(req_t type, const int socket_fd)
{
	switch (type)
		{
	case (FINISH) :
		SAY("Client concluded connection at " << socket_fd << '\n');
		break ;
	case (INVALID) :
		SAY("Invalid request. Closing\n");
		return ;
	case (GET) :
		SAY("Got a GET request!\n");
		break ;
	case (POST) :
		SAY("Got a POST request!\n");
		break ;
	case (HEAD) :
		SAY("Got a HEAD request!\n");
		break ;
	case (DELETE) :
		SAY("Got a DELETE request!\n");
		break ;
	default:
		break ;
	}
}

string	get_var_from_header(string msg, string name)
{
	size_t	varpos = msg.find(name);

	if (varpos == string::npos || varpos >= msg.find("\r\n\r\n"))
		return "";

	string a = msg.substr(varpos + name.size() + 2, msg.find("\r", varpos) - varpos - name.size() - 2);
	return a;
}

Server	*BetterSelect::findServByHostname(port_t port, string host)
{
	serv_list_t	&t = _used_ports[port];

	SAY("Looking for better match for host " << host << "... ");
	for (serv_list_t::iterator i = t.begin(); i != t.end(); ++i)
	{
		if ((*i)->getEnv(NAME).find(host) != string::npos)
		{
			SAY("Found at " << host << "!\n");
			return *i;
		}
	}
	SAY("Not found. Returning first of list (" << t.front()->getEnv(NAME) << ")!\n");
	return t.front();
}

req_t	BetterSelect::readMsg(int fd, connections_map_t::iterator &i)
{
	long	msg_len;

	SAY("Readimg from " << fd << "...\n");
	if (!(msg_len = recv(fd, _recv_buff, RECV_BUFF_SIZE, 0)))
		return FINISH;
	if (msg_len < 0)
		return timestamp("recv failed! ):\n", ERROR), INVALID;
	if (msg_len == RECV_BUFF_SIZE)
		throw HeadMsgTooLong();
	if (i->second->_is_sharing_port)
		i->second = findServByHostname(i->second->getPort(), get_var_from_header(_recv_buff, "Host"));
	return i->second->recieve(i->first, _recv_buff);
}

void	BetterSelect::_handleRequestResponse(fd_set &readfds, fd_set &writefds)
{
	Server	*t;

	if (FD_ISSET(_current_connection_fd, &writefds) && (t = _clis_map[_current_connection_fd])) //if the socket is ready to be written on
	{
		FD_CLR(_current_connection_fd, &_write_pool);
		if (!t->respond(_current_connection_fd))
			return (void)rmFd(_current_connection_fd, t);
		_timeout_map[_current_connection_fd] = time(NULL);
		return ;
	}
	for (connections_map_t::iterator i = _clis_map.begin(); i != _clis_map.end(); ++i)
	{
		if (!i->second)
			continue;
		if (FD_ISSET(i->first, &readfds)) //if the socket is ready to be read
		{
			req_t request_type	= readMsg(i->first, i);

			log_request(request_type, i->first);
			if (request_type == FINISH || request_type == INVALID || i->second->getRes() == _DONT_SEND)
			{
				rmFd(i->first, i->second);
				continue ;
			}
			_current_connection_fd = i->first;
			FD_SET(i->first, &_write_pool); //can allow next block to be executed
			_timeout_map[i->first] = time(NULL);
			return ;
		}
	}
}

void	BetterSelect::err_close_clis()
{
	for (connections_map_t::iterator i = _clis_map.begin(); i != _clis_map.end(); ++i)
		if (fcntl(i->first, F_GETFD) || !i->second)
			delResponseConnection(i->first);
	cout << "Called Err close clis\n";
}

void	BetterSelect::selectReadAndWrite()
{
	int				t;
	struct timeval	timeout = SEL_TIMEOUT;
	
	if (!_tot_size)
		return ;
	closeTimedOut();

	fd_set			readfds = _read_pool;
	fd_set			writefds = _write_pool;
	
	t = select(getBiggestFd() + 1, &readfds, &writefds, NULL, &timeout); // EXCEPTION -1
	if (t < 0)
		return (err_close_clis());
	_handleRequestResponse(readfds, writefds);
	_acceptNewConnections(readfds);
	return ;
}
