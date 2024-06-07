/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <fedmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/07 22:47:55 by fedmarti         ###   ########.fr       */
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
	//single instance
	static Webserv	_Singleton;
	
	string			_conf;
	static bool		_up;
	serv_list		_servers_up;
	serv_list		_servers_down;
	BetterSelect	_sel;
	conf_t			_env;

	Webserv(const Webserv &copy) {(void)copy;}
	Webserv&	operator=(const Webserv &assignment) {(void)assignment; return *this;}
	Webserv();
	// Webserv(const char *filename = DEFAULT_CONF);
	~Webserv();
public:
	static Webserv &getInstance();

	char	parseConfig();
	void	start();

	void	addServer(Server *s) {_servers_down.push_back(s);}

	static void	gracefullyQuit(int sig);

	void	upAllServers();
	void	downAllServers();
	
	const string	&getConf() const;
	void			setConf(string file_name);

	string getEnv( string key) const {return (_env.at(key));}
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