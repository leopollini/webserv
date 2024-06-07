/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/07 13:08:40 by lpollini         ###   ########.fr       */
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
	string			_conf;
	static bool		_up;
	serv_list		_servers_up;
	serv_list		_servers_down;
	BetterSelect	_sel;
	conf_t			_env;

	Webserv(const Webserv &copy) {(void)copy;}
	Webserv&	operator=(const Webserv &assignment) {(void)assignment; return *this;}
	Webserv() {};
public:
	Webserv(const char *filename = DEFAULT_CONF);
	~Webserv();

	char	parseConfig();
	void	start();

	void	addServer(Server *s) {_servers_down.push_back(s);}

	static void	gracefullyQuit(int sig);

	void	upAllServers();
	void	downAllServers();

	const string	&get_conf() const;

	class MissingConfigFile : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Config file missing";}
	};
	class FailedServerSetup : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Failed server setup";}
	};
};

#endif