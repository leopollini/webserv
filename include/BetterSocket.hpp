/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:01:08 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/02 16:39:59 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	BETTERSOCKET_HPP
# define BETTERSOCKET_HPP

# include "utils.hpp"

# define CLIENTS_MAX 10

struct	BetterSocket
{
	int					sock;
	struct sockaddr_in	client;
	socklen_t			len;

	int					fd;
	struct sockaddr_in	addr;
	
	void				init(short port, int address = INADDR_ANY)
	{
		while (0); // prevent inline-ing
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
	int					Accept()
	{
		len = 0;
		sock = accept(fd, (struct sockaddr *)&client, &len);
		return sock;
	}
	void	down()
	{
		close (fd);
	}

	BetterSocket() : sock(-1), fd(-1)  {}

	class FailedSocketCreation : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Socket creation failed";}
	};
	class FailedSocketBind : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Socket binding failed (port occupied?)";}
	};
	class FailedSocketListen : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Socket listen failed";}
	};
};

#endif