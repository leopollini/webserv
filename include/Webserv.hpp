/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/04 10:24:52 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP

# define WEBSERV_HPP

# include "utils.hpp"
# include "BetterSocket.hpp"
# include "BetterSelect.hpp"
# include "CGIManager.hpp"
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
	conf_t			_doc_types;


	Webserv(const Webserv &copy) : _cgi_man(_sel) {(void)copy;}
	Webserv&	operator=(const Webserv &assignment) {(void)assignment; return *this;}
	Webserv();
	// Webserv(const char *filename = DEFAULT_CONF);
	~Webserv();
	void	mapsInit();
public:
	CGIManager		_cgi_man;
	static Webserv &getInstance();

	char	parseConfig();
	void	start(char **prog_envp);
	void	stop() {_up = false;}

	static void	gracefullyQuit(int sig);

	void	upAllServers();
	void	downAllServers();
	
	const string	&getConf() const;
	void			setConf(string file_name);

	void	addServer(Server *s) {_servers_down.push_back(s);}
	string	&getEnv( string key) {return (_env[key]);}
	string	findDocType(const string &s) {return _doc_types[s];}

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