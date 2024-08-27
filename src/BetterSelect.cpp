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
	for (connections_map::iterator i = _clis_map.begin(); i != _clis_map.end(); i++)
		close(i->first);
	for (connections_map::iterator i = _servs_map.begin(); i != _servs_map.end(); i++)
		close(i->first);
}

void	BetterSelect::loadServFds(serv_list &ls)
{
	_tot_size = ls.size();
	_servs_map.clear();
	for (serv_list::iterator s = ls.begin(); s != ls.end(); s++)
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
	// cout << "added " << fd << "\n";
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
	for (connections_map::iterator i = _clis_map.begin(); i != _clis_map.end(); ++i)
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
// returns whether any connection has been closed
void	BetterSelect::closeTimedOut()
{
	if (!_clis_map.size())
		return ;
	for (connections_map::iterator i = _clis_map.begin(); i != _clis_map.end(); ++i)
		if (time(NULL) - _timeout_map[i->first] > CONNECTION_TIMEOUT && i->second)
			rmFd(i->first, i->second);
	return ;
}

void	BetterSelect::_acceptNewConnections(fd_set &read_fds)
{
	for (connections_map::iterator i = _servs_map.begin(); i != _servs_map.end(); i++)
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
		std::cout << "Client concluded connection at " << socket_fd << '\n';
		break ;
	case (INVALID) :
		std::cout << "Invalid request. Closing\n";
		return ;
	case (GET) :
		std::cout << "Got a GET request!\n";
		break ;
	case (POST) :
		std::cout << "Got a POST request!\n";
		break ;
	case (HEAD) :
		std::cout << "Got a HEAD request!\n";
		break ;
	case (DELETE) :
		std::cout << "Got a DELETE request!\n";
		break ;
	default:
		break ;
	}
}

void	BetterSelect::_handleRequestResponse(fd_set &readfds, fd_set &writefds)
{
	for (connections_map::iterator i = _clis_map.begin(); i != _clis_map.end(); ++i)
	{
		if (!i->second || i->first != _current_connection_fd)
			continue;
		if (FD_ISSET(i->first, &writefds)) //if the socket is ready to be written on
		{
			FD_CLR(i->first, &_write_pool);
			if (!i->second->respond(i->first))
			{
				rmFd(i->first, i->second);
				continue ;
			}
			_timeout_map[i->first] = time(NULL);
			return ;
		}
	}
	for (connections_map::iterator i = _clis_map.begin(); i != _clis_map.end(); ++i)
	{
		if (!i->second)
			continue;
		if (FD_ISSET(i->first, &readfds)) //if the socket is ready to be read
		{
			req_t request_type = i->second->recieve(i->first);

			log_request(request_type, i->first);
			if (request_type == FINISH || request_type == INVALID)
			{
				if (request_type == INVALID)
					cout << "Invalid request. Closing\n";
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
	for (connections_map::iterator i = _clis_map.begin(); i != _clis_map.end(); ++i)
		if (fcntl(i->first, F_GETFD) || !i->second)
			delResponseConnection(i->first);
}

void	BetterSelect::selectReadAndWrite()
{
	int				t;
	struct timeval	timeout = SEL_TIMEOUT;
	
	if (!_tot_size)
		return ;
	// closeTimedOut();

	fd_set			readfds = _read_pool;
	fd_set			writefds = _write_pool;
	
	t = select(getBiggestFd() + 1, &readfds, &writefds, NULL, &timeout); // EXCEPTION -1
	if (t <= 0)
		return (err_close_clis());
	_handleRequestResponse(readfds, writefds);
	_acceptNewConnections(readfds);
	return ;
}
