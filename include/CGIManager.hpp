/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIManager.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:47:44 by lpollini          #+#    #+#             */
/*   Updated: 2024/08/29 19:41:30 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIMANAGER_HPP
# define CGIMANAGER_HPP

# include "utils.hpp"
# include "BetterSelect.hpp"

struct BetterSelect;

class	CGIManager
{
	char 			**_env;
	BetterSelect	&_bspt;

	CGIManager&	operator=(const CGIManager &assignment) {(void)assignment; return *this;}
	CGIManager(const CGIManager &copy) {(void)copy;}
public :
	CGIManager() {}
	~CGIManager() {}

	void	start(const string cgi_path, std::list<string> args);
	// getter/setter via pointer
	char	***getEnv() {return &_env;}
	// getter/setter via pointer
	BetterSelect	*getSelect() {return _bspt;}
};

#endif