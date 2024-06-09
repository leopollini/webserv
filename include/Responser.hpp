/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Responser.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 15:00:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/09 20:48:36 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSER_HPP
# define RESPONSER_HPP

# include <iostream>
# include "utils.hpp"

class	Server;

class Responser
{
	Server		*_serv;
	string		_head;
	string		_body;
	string		_dir;
	location_t	*_loc;
	char		_file_flags;

public:
	bool		keepalive;
	short		_res_code;

	Responser(Server *s) : _serv(s) {}

	void	buildResponseHeader();
	void	buildResponseBody();

	location_t	*getLoc() {return _loc;}

	void	clear()
	{
		_head.clear();
		_body.clear();
		_dir.clear();
	}
	size_t	size()
	{
		return _head.size() + _body.size();
	}

	// Implement error for too long message bodies
	void	Send(int fd)
	{
		cout << "Trying to send " << size() << " bytes to " << fd << "...\n";
		send(fd, (_head + _body).c_str(), (size()), MSG_EOR);
	}
	Responser &operator=(const request_t &t)
	{
		_dir = t.dir;
		_loc = t.loc;
		return *this;
	}

	char	&getFileFlags() {return _file_flags;}
	string	&getDir() {return _dir;}
	string	badExplain(short code) {return "LOL";}
	size_t	getBodyLen() {return _body.size();}
	string	getResServer() {return "Lolserv";}
	string	getDocType();

	string	res_404();
};

#endif
