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

BetterSelect::~BetterSelect()
{
	FD_ZERO(&_read_pool);
	FD_ZERO(&_write_pool);
	for (connections_map::iterator i = _clis_map.begin(); i != _clis_map.end(); i++)
		close(i->first);
	for (connections_map::iterator i = _servs_map.begin(); i != _servs_map.end(); i++)
		close(i->first);
}

void	BetterSelect::loadServFds(std::list<Server *> &ls)
{
	_tot_size = ls.size();
	_servs_map.clear();
	for (std::list<Server *>::iterator s = ls.begin(); s != ls.end(); s++)
		if ((*s)->getState() == 1)
			addConnectionServ((*s)->getSockFd(), *s);
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
	_servs_map.erase(fd);
	_tot_size--;
}

void	BetterSelect::addListeningConnection(int fd, Server *s)
{
	FD_SET(fd, &_read_pool);
	_clis_map[fd] = s;
	_tot_size++;
}

void	BetterSelect::delListeningConnection(int fd)
{
	FD_CLR(fd, &_read_pool);
	_clis_map.erase(fd);
	_tot_size--;
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
	_clis_map.erase(fd);
	_tot_size--;
}

int	BetterSelect::getBiggestFd()
{
	if (!_clis_map.size())
		return (--_servs_map.end())->first;
	return std::max((--_servs_map.end())->first, (--_clis_map.end())->first);
}

// void _print_pool(fd_set pool, std::string name)
// {
//     std::cout << "Now printing " << name << "fd pool" << std::endl;
//     for ( uint i = 0; i < 1024; i++)
//     {
//         if (FD_ISSET(i, &pool))
//             std::cout << i << ' ';
//     }
//     std::cout << std::endl;
// }

void	BetterSelect::selectAndDo()
{
	int				t;
	fd_set			readfds = _read_pool;
	fd_set			writefds = _write_pool;
	struct timeval	timeout = SEL_TIMEOUT;
	
	if (!_tot_size)
		return ;
	t = select(getBiggestFd() + 1, &readfds, &writefds, NULL, &timeout); // EXCEPTION -1

	if (t <= 0)
		return ;
	for (connections_map::iterator i = _clis_map.begin(); i != _clis_map.end(); i++)
	{
		if (FD_ISSET(i->first, &writefds))
			i->second->recieve(i->first);
		if (FD_ISSET(i->first, &readfds))
		{
			switch (i->second->recieve(i->first))
			{
				case (FINISH) :
					std::cout << "Concluding connection at " << i->first << '\n';
					i->second->closeConnection(i->first);
					FD_CLR(i->first, &_read_pool);
				 break ;
				case (INVALID) :
					std::cout << "Invalid request. Closing\n";
					i->second->closeConnection(i->first);
					FD_CLR(i->first, &_read_pool);
				 break ;
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
			}
		}
	}
	for (connections_map::iterator i = _servs_map.begin(); i != _servs_map.end(); i++)
	{
		if (FD_ISSET(i->first, &readfds))
		{
			int fd = i->second->Accept();
			FD_SET(fd, &_read_pool);
			_clis_map[fd] = i->second;
			timestamp("Server at port " + itoa(i->second->getPort()) + " created new connection at fd " + itoa(fd) + '\n', INFO);
		}
		// else
		// 	timestamp("Apparently server at port " + itoa(i->second->getPort()) + " shut down. LOL\n", ERROR);
	}
	return ;
}