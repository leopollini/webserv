/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Responser.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 15:00:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/25 11:54:01 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSER_HPP
# define RESPONSER_HPP

# include <iostream>
# include "utils.hpp"
# include "CGIManager.hpp"

class	Server;

class	Webserv;

class Responser
{
	Server		*_serv;
	string		_head;
	string		_body;
	string		_dir;
	location_t	*_loc;
	char		_file_flags;
	conf_t		_extra_args;
	
public:

	short		_res_code;
	bool		keepalive;

// use 0 for no return, 1 for regular return, -1 for cgi return
	char		_is_returning;

	Responser(const Responser &a) {(void)a;}
	Responser(Server *s) : _serv(s), _loc(NULL), _is_returning(false) {}
	~Responser() {}
	Responser &operator=(const request_t &t);

	void	buildResponseHeader();
	char	buildResponseBody();

	char	internalServerError();

	location_t	*getLoc() const {if (!_loc) SAY("###getLoc() got a NULL??\n"); return _loc;}

	void	clear();
	size_t	size() {return _head.size() + _body.size();}

	// Implement error for too long message bodies
	void	Send(int fd);

	char	&getFileFlags() {return _file_flags;}
	string	&getDir() {return _dir;}
	string	badExplain(short code) {(void) code; return "LOL";}		// THIS IS USELESS
	size_t	getBodyLen() {return _body.size();}
	// 	string	getResServer() {return "Lolserv";}
	string	getDocType();

	class LocNull : public std::exception
	{
		const char *what() const throw()
		{
			return ("For some reason, _loc inside a server was NULL");
		}

	};
};

#endif
