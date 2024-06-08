/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Responser.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 15:00:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/08 20:05:14 by lpollini         ###   ########.fr       */
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

public:
	short		_res_code;

	Responser(Server *s) : _serv(s) {}

	void	buildResponseHeader();
	void	buildResponseBody();


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
	void	Send(int fd)
	{
		send(fd, (_head + _body).c_str(), (size()), MSG_EOR);
	}
	Responser &operator=(const request_t &t)
	{
		_dir = t.dir;
		_loc = t.loc;
		return *this;
	}



	string	badExplain(short code) {return "OK";}
	string	getDocType() {return "text/html";}
	size_t	getBodyLen() {return _body.size();}
	string	getResServer() {return "Lolserv";}
};

#endif
