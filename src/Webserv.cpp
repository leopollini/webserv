/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:36 by lpollini          #+#    #+#             */
/*   Updated: 2024/05/31 19:25:40 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Webserv.hpp"

bool	Webserv::_up;

Webserv::Webserv(const string &filename) : _conf(filename), _servers_up()
{
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
	timestamp("Parsing config file!\n",YELLOW);
	if ((_conf_fd = open(_conf.c_str(), O_RDONLY)) < 0)
		throw MissingConfigFile();

	addServer(new Server(8080));
	
	return 0;
}

void	Webserv::start()
{
	std::deque<Server *>	ok_servs;

	timestamp("Starting Webserv!\n",GREEN);
	_up = true;
	upAllServers();
	while (_up)
	{
		if (!_servers_up.size())
		{
			timestamp("No server is accessible! Sleeping.\n", YELLOW);
			usleep(NO_SERVER_SLEEP_TIME_MS * 1000);
			// something to unclog the system here, dunno
			continue ;
		}
		else if (_servers_down.size())
		{
			timestamp("Some servers are still down! Retrying.\n", YELLOW);
			upAllServers();
		}
		ok_servs = _poll.Poll();
		for (size_t i = 0; i < ok_servs.size(); i++)
			ok_servs[i]->Accept(_poll.findPollfd(ok_servs[i]->getSockFd()));
		usleep(20000);
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

	timestamp("\b\bGracefully shutting Webserv!\n\t\tSend signal again to Force Close\n",GRAYI);
	signal(SIGINT, SIG_DFL);
	_up = false;
}

void	Webserv::upAllServers()
{
	for (int c = 0; c < DOWN_SERVER_TRIES_MAX && _servers_down.size() && _up; c++)
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
				timestamp("Failed to setup Server at " + itoa((*i)->getPort()) + ": " + string(e.what()) + '\n', ERROR);
				(*i)->_down_count++;
			}
			usleep(DOWN_SERVER_SLEEP_MS * 1000);
		}
	}
	_poll.loadFds(_servers_up);
}

void	Webserv::downAllServers()
{
	for (std::list<Server *>::iterator i = _servers_up.begin(); i != _servers_up.end(); i++)
		(*i)->down();
	_servers_down.insert(--_servers_down.end(), _servers_up.begin(), _servers_up.end());
	_servers_up.clear();
}
