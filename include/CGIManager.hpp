/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIManager.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:47:44 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/27 14:21:02 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIMANAGER_HPP
# define CGIMANAGER_HPP

# include "utils.hpp"
# include "BetterEnv.hpp"

class	Webserv;

struct	BetterSelect;

class	Server;

// Unique. Found inside Webserv
struct	CGIManager
{
	char 			**_envp;
	BetterSelect	&_bs;
	pid_time_lst	_pids;

	// DON'T USE
	CGIManager&	operator=(const CGIManager &assignment) {(void)assignment; return *this;}
	// DON'T USE
	CGIManager(const CGIManager &copy) : _bs(copy._bs) {(void)copy;}
	// DON'T USE
	CGIManager(BetterSelect &bs) : _bs(bs) {}
	~CGIManager()
	{
		for (pid_time_lst::iterator i = _pids.begin(); i != _pids.end(); ++i)
			kill(i->first, SIGKILL);
	}

	// All cariadic args MUST be char * and the last arg MUST be a NULL
	void	start(Server *s, const string &cgi_dir, const string &uri_dir, string query_string = "", string body = "");
	void	envSet(char **a) {_envp = a;}
	void	purgeCGI();
};

#endif