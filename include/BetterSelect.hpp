/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterSelect.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:47:44 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/01 14:34:26 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BETTERSELECT_HPP
# define BETTERSELECT_HPP

# include "Server.hpp"
# include "BetterSocket.hpp"
# include "utils.hpp"

# define POLL_TIMEOUT 1000

struct	BetterSelect
{
	fd_set			_all_fds;	
	short			_tot_size;
	connections_map	_servs_map;
	connections_map	_clis_map;

	BetterSelect() : _tot_size(0) {FD_ZERO(&_all_fds);}
	~BetterSelect();

	void			loadServFds(std::list<Server *> &ls);
	void			addConnectionCli(int fd, Server *s);
	void			delConnectionCli(int pfd);
	void			addConnectionServ(int fd, Server *s);
	void			delConnectionServ(int pfd);
	void			selectAndDo();
	int				getBiggestFd();

};

#endif