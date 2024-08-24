/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Responser.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/08 15:00:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/08/24 18:45:58 by lpollini         ###   ########.fr       */
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
	conf_t		_extra_args;

public:

	class LocNull : public std::exception
	{
		const char *what() const throw()
		{
			return ("For some reason, _loc inside a server was NULL");
		}

	};
	bool		keepalive;
	short		_res_code;
	bool		_is_redirecting;

	Responser(Server *s) : _serv(s), _loc(NULL), _is_redirecting(false) {}

	void	buildResponseHeader();
	void	buildResponseBody();

	location_t	*getLoc() const {if (!_loc) cout << "getLoc() got a NULL??\n"; return _loc;}

	void	clear()
	{
		_head.clear();
		_body.clear();
		_dir.clear();
		_extra_args.clear();
		_is_redirecting = false;
	}
	size_t	size()
	{
		return _head.size() + _body.size();
	}

	// Implement error for too long message bodies
	void	Send(int fd)
	{
		cout << "Trying to send " << size() << " bytes to " << fd << "... ";
		send(fd, (_head + _body).c_str(), (size()), MSG_EOR);
		timestamp("Done!\n", DONE, BOLD, false);
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
