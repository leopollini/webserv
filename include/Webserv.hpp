/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/01 16:34:26 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP

# define WEBSERV_HPP

# include "utils.hpp"
# include "BetterSocket.hpp"
# include "BetterSelect.hpp"
# include "Server.hpp"
# include "Webserv.hpp"

class	Server;

struct	BetterSelect;

class	Webserv
{
	const string		_conf;
	int					_conf_fd;
	static bool			_up;
	std::list<Server *>	_servers_up;
	std::list<Server *>	_servers_down;
	struct BetterSelect	_sel;
	conf_t				_env;

	Webserv(const Webserv &copy) {(void)copy;}
	Webserv&	operator=(const Webserv &assignment) {(void)assignment; return (*this);}
	Webserv() {};
public:
	Webserv(const string &filename);
	~Webserv();

	char	parseConfig();
	void	start();

	void	addServer(Server *s);

	static void	gracefullyQuit(int sig);

	void	upAllServers();
	void	downAllServers();

	class MissingConfigFile : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Config file missing";}
	};
};

#endif