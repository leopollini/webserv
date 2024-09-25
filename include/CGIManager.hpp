/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIManager.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <fedmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:47:44 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/26 00:15:02 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIMANAGER_HPP
# define CGIMANAGER_HPP

# include "utils.hpp"

struct	BetterSelect;
class	Server;

// Unique. Found inside Webserv
struct	CGIManager
{
	
	char 				**_env;
	BetterSelect		&_bs;
	std::list<pid_t>	_pids;

	// DON'T USE
	CGIManager&	operator=(const CGIManager &assignment) {(void)assignment; return *this;}
	// DON'T USE
	CGIManager(const CGIManager &copy) : _bs(copy._bs) {(void)copy;}
	// DON'T USE
	CGIManager(BetterSelect &bs) : _bs(bs) {}
	~CGIManager()
	{
		for (std::list<pid_t>::iterator i = _pids.begin(); i != _pids.end(); ++i)
			kill(*i, SIGKILL);
	}

	// All cariadic args MUST be char * and the last arg MUST be a NULL
	void	start(Server *s, const string &cgi_dir, const string &uri_dir);
};

#endif