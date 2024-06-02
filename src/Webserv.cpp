/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:36 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/02 18:09:10 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

bool	Webserv::_up;

Webserv::Webserv(const char *filename) : _servers_up()
{
	if (!filename)
		_conf = string(DEFAULT_CONF);
	else
		_conf = string(filename);
	timestamp("Setting up Webserv!\n", CYAN);
	signal(SIGINT, gracefullyQuit);
}

Webserv::~Webserv()
{
	timestamp("Destroying Webserv!\n", BLUE);
	close(_conf_fd);
	for (std::list<Server *>::iterator i = _servers_up.begin(); i != _servers_up.end(); i++)
		delete *i;
	for (std::list<Server *>::iterator i = _servers_down.begin(); i != _servers_down.end(); i++)
		delete *i;
}

char	Webserv::parseConfig()
{
	if ((_conf_fd = open(_conf.c_str(), O_RDONLY)) < 0)
		throw MissingConfigFile();

	timestamp("Parsing config file!\n",YELLOW);

	addServer(new Server(8080));


	close(_conf_fd);
	return 0;
}

void	Webserv::start()
{
	timestamp("Starting Webserv!\n",GREEN);
	_up = true;
	upAllServers();
	while (_up)
	{
		if (!_servers_up.size())
		{
			std::cout << "No servers up!\n";
			sleep(2);
			continue ;
		}
		cout << "Waitimg.\n";
		_sel.selectAndDo();

		usleep(200000);
	}
	downAllServers();
	_up = false;
}

void	Webserv::addServer(Server *s)
{	
	_servers_down.push_front(s);
}

void	Webserv::gracefullyQuit(int sig)
{
	(void)sig;

	timestamp("\b\bGracefully shutting Webserv! Send signal again to Force Close\n", GRAYI);
	signal(SIGINT, SIG_DFL);
	_up = false;
}

void	Webserv::upAllServers()	// PLEASE REDO
{
	for (std::list<Server *>::iterator i = _servers_down.begin(); i != _servers_down.end() && _up; i++)
	{
		try
		{
			(*i)->up();
			(*i)->_down_count = 0;
			_servers_up.push_front(*i);
			_servers_down.erase(i, ++i);
			std::advance(i, -2);
		}
		catch(const std::exception& e)
		{
			if ((*i)->_down_count + 1 >= DOWN_SERVER_TRIES_MAX)
				continue ;
			timestamp("Failed to setup Port " + itoa((*i)->getPort()) + ": " + string(e.what()) + '\n', ERROR);
			(*i)->_down_count++;
		}
	}
	_sel.loadServFds(_servers_up);
}

void	Webserv::downAllServers()
{
	for (std::list<Server *>::iterator i = _servers_up.begin(); i != _servers_up.end(); i++)
		(*i)->down();
	_servers_down.insert(--_servers_down.end(), _servers_up.begin(), _servers_up.end());
	_servers_up.clear();
}
