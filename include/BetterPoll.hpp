/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterPoll.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:47:44 by lpollini          #+#    #+#             */
/*   Updated: 2024/05/30 18:01:01 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BETTERPOLL_HPP
# define BETTERPOLL_HPP

# include "Server.hpp"
# include "BetterSocket.hpp"
# include "utils.hpp"

# define POLL_TIMEOUT 1000

struct	BetterPoll
{
	pollfd					*_fds;
	short					_size;
	std::map<int, Server *>	_map;

	BetterPoll() : _fds(NULL) {}
	~BetterPoll() {delete[] _fds;}

	void					loadFds(std::list<Server *> &ls);
	std::deque<Server *>	Poll();
	pollfd					&findPollfd(int fd);
};

#endif