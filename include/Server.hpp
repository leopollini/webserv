/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/05/31 15:33:30 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP

# define SERVER_HPP

# include "BetterSocket.hpp"
# include <stdio.h>

typedef	short port_t;

class	Server
{
	int					_clientfd;

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
	void	Accept(pollfd pfd);

	bool	tryup();
	void	up();
	void	down();
	char	getState() {return _state;}
};

#endif