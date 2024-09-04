/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIManager.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:47:44 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/04 15:31:54 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIMANAGER_HPP
# define CGIMANAGER_HPP

# include "utils.hpp"

struct	BetterSelect;
class	Server;

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
	~CGIManager() {}

	// All cariadic args MUST be char * and the last arg MUST be a NULL
	void	start(Server *s, const string cgi_path, const string &arg);
};

#endif