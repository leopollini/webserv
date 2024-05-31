/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterPoll.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:57:55 by lpollini          #+#    #+#             */
/*   Updated: 2024/05/30 18:02:05 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/BetterPoll.hpp"

void BetterPoll::loadFds(std::list<Server *> &ls)
{
	int	i = 0;

	_size = ls.size();
	delete _fds;
	if (!_size)
		return (void)(_fds = NULL);
	_fds = new pollfd[_size];
	_map.clear();
	for (std::list<Server *>::iterator s = ls.begin(); s != ls.end(); s++, i++)
	{
		if ((*s)->getState() == -1)
			continue ;
		_fds[i].fd = (*s)->getSockFd();
		_fds[i].events = POLLIN | POLLOUT;
		_map.insert(std::pair<int, Server *>(_fds[i].fd, *s));
	}
}

std::deque<Server *>	BetterPoll::Poll()
{
	int						t;
	std::deque<Server *>	res;
	
	if (!_fds)
		return res;
	t = poll(_fds, _size, POLL_TIMEOUT);
	if (t >= 0)
		for (int i = 0; i < _size; i++)
			if (_fds[i].revents)
				res.push_front(_map[_fds[i].fd]);
	return res;
}

pollfd	&BetterPoll::findPollfd(int fd)
{
	for (int i = 0; i < _size; i++)
		if (fd == _fds[i].fd)
			return (_fds[i]);
}
