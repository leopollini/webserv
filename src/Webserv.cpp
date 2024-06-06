/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:36 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/06 17:25:13 by lpollini         ###   ########.fr       */
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
	// for (serv_list::iterator i = _servers_up.begin(); i != _servers_up.end(); i++)
	// 	delete *i;
	for (serv_list::iterator i = _servers_down.begin(); i != _servers_down.end(); i++)
		delete *i;
}

void	fill_line(conf_t *env, list<Parsing::token>::iterator &s)
{
	string	&t = (*env)[s->content];

	while ((++s)->content != ";" && s->content != "{")
		t.append(s->content + ' ');
	t[t.size() - 1] = '\0';
}

char	Webserv::parseConfig()
{
	// if ((_conf_fd = open(_conf.c_str(), O_RDONLY)) < 0)
		// throw MissingConfigFile();

	timestamp("Parsing config file!\n",YELLOW);
	
	string fileContent = Parsing::read_file(get_conf());
	list<Parsing::token> tokens = Parsing::tokenize(fileContent);
	Parsing::print_tokens(tokens);
	
	timestamp("Beginning servers configuration!\n", INFO);
	Server		*current;
	conf_t		*env;
	location_t	*current_loc;
	conf_t		*env_loc;
	short		brackets = 0;
	int			ln;
	int			servs = 0;
	for (list<Parsing::token>::iterator i = tokens.begin(); i != tokens.end(); ++i)
	{
		if (!i->content.size())
			continue ;
		if (i->content == NEW_SERVER)
		{
			if (brackets++ > 0 || (++i)->content != "{" )
				throw Parsing::ErrorType();
			++i;
			current = new Server(servs);
			addServer(current);
			env = current->getEnv();
			servs++;
			while (brackets == 1 && i != tokens.end())
			{
				if (i->content == LOCATION)
				{
					current_loc = new location_t;
					current->addLocation(current_loc);
					env_loc = &current_loc->stuff;
					fill_line(env_loc, i);
					if (brackets++ > 1 || (i)->content != "{" )
						throw Parsing::ErrorType();
					++i;
					while (brackets == 2 && i != tokens.end())
					{
						if (i->content == "}" && brackets-- && ++i != tokens.end())
							break ;
						fill_line(env_loc, i);
						++i;
					}
				}
				if (i->content == "}" && !--brackets)
					break ;
				fill_line(env, i);
				++i;
			}
		}
	}
	// for (serv_list::iterator i = _servers_down.begin(); i != _servers_down.end(); i++)
	// 	(*i)->locReadEnv();
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
		(*i)->printServerStats();
		try
		{
			(*i)->up();
			(*i)->_down_count = 0;
			_servers_up.push_front(*i);
			_servers_down.erase(std::prev(++i), i);
		}
		catch(const std::exception& e)
		{
			if (++(*i)->_down_count >= DOWN_SERVER_TRIES_MAX)
				continue ;
			timestamp("Failed to setup Port " + itoa((*i)->getPort()) + ": " + string(e.what()) + '\n', ERROR);
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