/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterSocket.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 09:48:35 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/04 18:50:38 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/BetterSocket.hpp"

int	BetterSocket::Accept()
{
	len = 0;
	sock = accept(fd, (struct sockaddr *)&client, &len);
	return sock;
}

void	BetterSocket::init(short port, int address)
{
	_port = port;
	while (0); // prevent inline-ing
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
