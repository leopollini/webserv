/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/15 02:54:13 by fedmarti         ###   ########.fr       */
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
	conf_t			_doc_types;

	Webserv(const Webserv &copy) {(void)copy;}
	Webserv&	operator=(const Webserv &assignment) {(void)assignment; return *this;}
	Webserv();
	// Webserv(const char *filename = DEFAULT_CONF);
	~Webserv();
public:
	char 			read_buff[BUFFER_SIZE + 1];
	static Webserv &getInstance();

	void	docTypesInit();
	char	parseConfig();
	void	start();

	void	addServer(Server *s) {_servers_down.push_back(s);}

	static void	gracefullyQuit(int sig);

	void	upAllServers();
	void	reviveServers(ulong retry_time = SERVER_RETRY_TIME);
	void	downAllServers();
	void	downServer(Server *serv);
	void	downServer(int fd);

	static int	socketRead(int fd, char **dest, size_t size = BUFFER_SIZE);

	
	const string	&getConf() const;
	void			setConf(string file_name);

	string	&getEnv( string key) {return (_env[key]);}
	string	findDocType(const string &s) {return _doc_types.find(s)->second;}

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