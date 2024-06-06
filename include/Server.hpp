/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/04 11:24:33 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP

# define SERVER_HPP

# include "BetterSocket.hpp"
# include <stdio.h>

# define HEAD_BUFFER 3000

struct request_t
{
	req_t	type;
	string	dir;
};

struct	Response
{
	string	resp;
	
};


class	Server
{
	fd_list			_clientfds;

	port_t			_port;
	BetterSocket	_sock;
	char			_state;
	conf_t			_env;
	char			_recieved_head[HEAD_BUFFER];
	uint			_msg_len;
	request_t		_current_request;

	Server&	operator=(const Server &assignment) {(void)assignment; return *this;}
	Server(const Server &copy) {(void)copy;}
public:
	int					_down_count;
	conf_t				_sconf;
	

	Server(port_t port);
	Server() {};
	~Server();

	int		getSockFd();
	int		getPort() {return _port;}
	char	getState() {return _state;}
	void	setPort(port_t port) {_port = port;}
	int		Accept();

	bool	tryup();
	void	up();
	void	down();
	req_t	recieve(int fd);
	void	respond(int fd);
	void	closeConnection(int fd);
	req_t	parseMsg(int fd);

	void	printHttpRequest(string &msg, int fd_from);
	
	class HeadMsgTooLong : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Recieved too long a request";}
	};
};

#endif