/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <fedmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/27 19:03:27 by fedmarti         ###   ########.fr       */
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
# include "BetterEnv.hpp"



class	Server;

struct	BetterSelect;

class	Webserv
{
	//single instance
	static Webserv	_Singleton;

	conf_t			_env;
	ecode_t			_bad_explain;
	conf_t			_doc_types;
	
	string			_conf;
	serv_list_t		_servers_up;
	serv_list_t		_servers_down;
	BetterSelect	_sel;


	Webserv(const Webserv &copy) : _cgi_man(_sel) {(void)copy;}
	Webserv&	operator=(const Webserv &assignment) {(void)assignment; return *this;}
	Webserv();
	// Webserv(const char *filename = DEFAULT_CONF);
	~Webserv();
	void	docTypesInit();
public:
	static char		_up;
	char 			read_buff[BUFFER_SIZE + 1];
	CGIManager		_cgi_man;
	static Webserv	&getInstance();

	void	superPipeSet(int super = 0, string body = "");
	char	parseConfig();
	void	start(char **prog_envp);
	void	stop() {_up = 0;}
	void	lastSend() {_up = -1;}

	static void	gracefullyQuit(int sig);

	void	upAllServers();
	void	reviveServers(ulong retry_time = SERVER_RETRY_TIME);
	void	downAllServers();
	void	downServer(Server *serv);
	void	downServer(int fd);

	// static int	socketRead(int fd, char **dest, size_t size = BUFFER_SIZE);

	
	const string	&getConf() const;
	void			setConf(string file_name);

	void	addServer(Server *s);
	string	&getEnv(string key) {return (_env[key]);}

	string	findDocType(const string &s) {return _doc_types[s];}
	string	badExplain(short code) {return _bad_explain.count(code) ? _bad_explain[code] : ((code == 204)? "No Content": "OK");}

	void	deleteConnection(int fd) {_sel.delListeningConnection(fd); _sel.delResponseConnection(fd); close(fd);}
	void	fill_line(conf_t *env, list<Parsing::token>::iterator &s);

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
	class InvalidDirectiveOverwrite : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Cannot overwrite listen directive";}
	};
};

#endif