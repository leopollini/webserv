/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <fedmarti@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:36 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/05 17:53:46 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "parsing_util.hpp"

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
	for (serv_list::iterator i = _servers_up.begin(); i != _servers_up.end(); i++)
		delete *i;
	for (serv_list::iterator i = _servers_down.begin(); i != _servers_down.end(); i++)
		delete *i;
}

char	Webserv::parseConfig()
{
	// if ((_conf_fd = open(_conf.c_str(), O_RDONLY)) < 0)
		// throw MissingConfigFile();

	timestamp("Parsing config file!\n",YELLOW);
	
	string fileContent = Parsing::read_file(get_conf());
	list<Parsing::token> tokens = Parsing::tokenize(fileContent);
	// Parsing::print_tokens(tokens);
	
	addServer(new Server(8080));
	addServer(new Server(8081));
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
		// cout << "Waitimg.\n";
		_sel.selectAndDo();

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

	timestamp("\b\bGracefully shutting Webserv! Send signal again to Force Close\n", GRAYI);
	signal(SIGINT, SIG_DFL);
	_up = false;
}

void	Webserv::upAllServers()	// PLEASE REDO
{
	for (serv_list::iterator i = _servers_down.begin(); i != _servers_down.end() && _up; i++)
	{
		try
		{
			(*i)->up();
			(*i)->_down_count = 0;
			_servers_up.push_front(*i);
			_servers_down.erase(i, ++i);
			std::advance(i, -3);
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
	for (serv_list::iterator i = _servers_up.begin(); i != _servers_up.end(); i++)
		(*i)->down();
	_servers_down.insert(--_servers_down.end(), _servers_up.begin(), _servers_up.end());
	_servers_up.clear();
}

const string	&Webserv::get_conf() const
{
	return (_conf);
}