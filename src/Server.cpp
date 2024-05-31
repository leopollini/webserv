/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:08:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/05/31 19:31:36 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"

Server::Server(port_t port) : _port(port), _down_count(0), _state(0)
{
	timestamp("Added server at port " + itoa(_port) + "!\n", CYAN);
	
}

Server::~Server()
{
	timestamp("Server at " + itoa(_port) + " removed!\n", BLUE);
	close(_sock.fd);
	if (_sock.sock >= 0)
		close(_sock.sock);
}

int	Server::getSockFd()
{
	return _sock.fd;
}

void	Server::Accept(pollfd pfd)
{
	_clientfd = _sock.Accept();

	getsockname(_sock.sock, (sockaddr *)&_sock.client, &_sock.len);
	
	timestamp("Server at " + itoa(_port) + " caught a client! IP: " + addr_to_str(_sock.client.sin_addr.s_addr) + '\n', MAGENTA);
	write(_clientfd, "Hahalol\n", 8);
	
	// fcntl(_clientfd, F_SETFL, fcntl(_clientfd, F_GETFL, 0) | O_NONBLOCK);
	
	close(_clientfd);
	close(_sock.sock);
}

bool	Server::tryup()
{
	_state = 0;
	if (!_state)
		up();
	return 1;
}

void	Server::up()
{
	_sock.init(_port);
	timestamp("Port " + itoa(_port) + " now open!\n", CYAN);
	_state = 1;
}

void	Server::down()
{
	if (!_state)
		return ;
	_sock.down();
	timestamp("Port " + itoa(_port) + " now closed!\n", YELLOW);
	_state = 0;
}
