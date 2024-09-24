/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/15 02:44:25 by fedmarti          #+#    #+#             */
/*   Updated: 2024/09/24 18:51:06 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BetterSocket.hpp"
#include "Webserv.hpp"




void BetterSocket::init(short port, int address)
{

	if (fd >= 0)
		close(fd);
	if ((fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
		throw FailedSocketCreation();
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = address;
	addr.sin_port = htons(port);
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		throw FailedSocketBind();
	if (listen(fd, CLIENTS_MAX) < 0)
		throw FailedSocketListen();
}


int BetterSocket::Accept()
{
	len = 0;
	sock = accept(fd, (struct sockaddr *)&client, &len);
	return sock;
}
