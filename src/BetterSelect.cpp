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
	FD_ZERO(&_all_fds);
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
	FD_SET(fd, &_all_fds);
	_servs_map[fd] = s;
	_tot_size++;
}

void	BetterSelect::delConnectionServ(int fd)
{
	FD_CLR(fd, &_all_fds);
	_servs_map.erase(fd);
	_tot_size--;
}

void	BetterSelect::addConnectionCli(int fd, Server *s)
{
	FD_SET(fd, &_all_fds);
	_clis_map[fd] = s;
	_tot_size++;
}

void	BetterSelect::delConnectionCli(int fd)
{
	FD_CLR(fd, &_all_fds);
	_clis_map.erase(fd);
	_tot_size--;
}

int	BetterSelect::getBiggestFd()
{
	if (!_clis_map.size())
		return (--_servs_map.end())->first;
	return std::max((--_servs_map.end())->first, (--_clis_map.end())->first);
}

void	BetterSelect::selectAndDo()
{
	int				t;
	fd_set			readfds = _all_fds;
	fd_set			writefds = _all_fds;
	struct timeval	timeout = {1, 0};
	
	if (!_tot_size)
		return ;
	t = select(getBiggestFd() + 1, &readfds, &writefds, NULL, &timeout);
	if (!t)
		return ;
	for (connections_map::iterator i = _servs_map.begin(); i != _servs_map.end(); i++)
	{
		if (FD_ISSET(i->first, &readfds))
			FD_SET(i->second->Accept(), &_all_fds);
		else
			timestamp("Apparently server at port " + itoa(i->second->getPort()) + " shut down. LOL\n", ERROR);
		cout << "(serv) FD " << i->first << '\n';
	}
	for (connections_map::iterator i = _clis_map.begin(); i != _clis_map.end(); i++)
	{
		if (FD_ISSET(i->first, &readfds)) // || FD_ISSET(i->first, &writefds))
			i->second->doServerStuff(i->first);
		else
			i->second->closeConnection(i->first);
		cout << "(clo) FD " << i->first << '\n';
	}
	return ;
}
