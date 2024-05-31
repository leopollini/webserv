/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/05/31 15:47:59 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP

# define WEBSERV_HPP

# include "utils.hpp"
# include "BetterSocket.hpp"
# include "BetterPoll.hpp"
# include "Server.hpp"

# define DOWN_SERVER_TRIES_MAX 5
# define DOWN_SERVER_SLEEP_MS 200
# define DOWN_SERVER_MAX 10
# define NO_SERVER_SLEEP_TIME_MS 3000

class	Server;

struct	BetterPoll;

class	Webserv
{
	const std::string	_conf;
	int					_conf_fd;
	static bool			_up;
	std::list<Server *>	_servers_up;
	std::list<Server *>	_servers_down;
	struct BetterPoll	_poll;
	conf_t				_env;

	Webserv(const Webserv &copy) {(void)copy;}
	Webserv&	operator=(const Webserv &assignment) {(void)assignment; return (*this);}
	Webserv() {};
public:
	Webserv(const std::string &filename);
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