/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/01 16:58:54 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP

# define SERVER_HPP

# include "BetterSocket.hpp"
# include <stdio.h>

class	Server
{
	std::list<int>		_clientfds;

	port_t				_port;
	struct BetterSocket	_sock;
	char				_state;
	conf_t				_env;

	Server&	operator=(const Server& assignment) {(void)assignment; return (*this);}
	Server(const Server& copy) {(void)copy;}
public:
	int					_down_count;
	conf_t				_sconf;
	

	Server(port_t port);
	~Server();

	int		getSockFd();
	int		getPort() {return _port;}
	int		Accept();

	bool	tryup();
	void	up();
	void	down();
	char	getState() {return _state;}
	void	doServerStuff(int fd)
	{
		std::cout << "Called server at port " << _port << "!\n";
	}
	void	closeConnection(int fd)
	{
		std::cout << "Closing connection at port  " << itoa(fd) << ", fd: " + itoa(fd) << "!\n";
		close(fd);
	}
};

#endif