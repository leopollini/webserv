/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterSocket.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:01:08 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/27 02:39:40 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef	BETTERSOCKET_HPP
# define BETTERSOCKET_HPP

# include "utils.hpp"
# include "definitions.hpp"
# include <cstring>

# define CLIENTS_MAX 10
# define BUFFER_SIZE 3000ul

struct	BetterSocket
{
private:
	size_t	_buffer_len;
	char	_read_buff[BUFFER_SIZE + 1];
	string	_flush_bytes(size_t n);
	bool	_successful_read;
public:
	size_t	sockRead(int input_fd);
	int					sock;
	struct sockaddr_in	client;
	socklen_t			len;
	int					fd;
	struct sockaddr_in	addr;
	
	BetterSocket&	operator=(const BetterSocket &assignment) {(void)assignment; return *this;}
	BetterSocket(const BetterSocket &copy) {(void)copy;}
	BetterSocket() : sock(-1), fd(-1)  {}
	void	flushBuffer();
	void	flushUntilEnd();
	string	getLine();
	bool	wasReadSuccessful() const {return _successful_read;};
	string	getBytes(size_t to_read);
	string	getChunk();
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