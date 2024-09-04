/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:01:08 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/04 18:50:26 by lpollini         ###   ########.fr       */
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
	serv_list_t			_servs;
	port_t				_port;

	BetterSocket&	operator=(const BetterSocket &assignment) {(void)assignment; return *this;}
	BetterSocket(const BetterSocket &copy) {(void)copy;}
	BetterSocket() : sock(-1), fd(-1)  {}
	~BetterSocket() {down();}
	
	void				init(short port, int address = INADDR_ANY);
	int					Accept();
	void				down() {close (fd);}


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