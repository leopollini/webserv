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

# define SEL_TIMEOUT {1, 0}

struct	BetterSelect
{
	fd_set			_read_pool;
	fd_set			_write_pool;
	short			_tot_size;
	connections_map	_servs_map;
	connections_map	_clis_map;

	BetterSelect();
	~BetterSelect();

	void			loadServFds(serv_list &ls);

	void			addListeningConnection(int fd, Server *s);
	void			delListeningConnection(int pfd);
	void			addResponseConnection(int fd, Server *s);
	void			delResponseConnection(int pfd);
	void			addConnectionServ(int fd, Server *s);
	void			delConnectionServ(int pfd);
	
	void			selectAndDo();
	void			postSelect(fd_set &readfds, fd_set &writefds);
	int				getBiggestFd();

};

#endif