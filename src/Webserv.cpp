/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:36 by lpollini          #+#    #+#             */
/*   Updated: 2024/08/24 18:14:22 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "parsing_util.hpp"

bool	Webserv::_up = false;
Webserv Webserv::_Singleton;

Webserv &Webserv::getInstance()
{
	return (_Singleton);
}

Webserv::Webserv() : _conf(DEFAULT_CONF)
{
	timestamp("Setting up Webserv!\n", CYAN);
	docTypesInit();
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

// add anything useful. Every not recgnized extension is mapped to "default", which tells the browser to download the file
void	Webserv::docTypesInit()
{
	_doc_types[".html"] = "text/html";
	_doc_types[".css"] = "text/css";
	_doc_types[".cpp"] = "text/html";
	_doc_types[".hpp"] = "text/html";
}

void	fill_line(conf_t *env, list<Parsing::token>::iterator &s)
{
	if (s->content.empty())
		return ;

	string	&t = (*env)[s->content];
	t.clear();
	while ((++s)->type != ';' && s->type != '{')
		t.append(s->content + ' ');
	t = t.substr(0, t.size() - 1);
}

// Very ugly. Sorry, just don't look at it.
// Format be: env[FIRST_WORD] = TRAILING WORDS SEPARATED BY SPACES (no trailing space :) )
char	Webserv::parseConfig( void )
{
	// if ((_conf_fd = open(_conf.c_str(), O_RDONLY)) < 0)
		// throw MissingConfigFile();

	timestamp("Parsing config file!\n",YELLOW);
	
	string fileContent = Parsing::read_file(getConf());
	list<Parsing::token> tokens = Parsing::tokenize(fileContent);
	// Parsing::print_tokens(tokens);
	
	timestamp("Beginning servers configuration!\n", INFO);
	Server		*current;
	conf_t		*env;
	location_t	*current_loc;
	conf_t		*env_loc;
	short		brackets = 0;
	int			servs = 0;
	for (list<Parsing::token>::iterator i = tokens.begin(); i != tokens.end(); ++i)
	{
		if (i->content.empty() || i->type == '}' || i->content == "http")
			continue ;
		if (i->content == NEW_SERVER)
		{
			if (brackets++ > 0 || (++i)->type != '{' )
				throw Parsing::ErrorType();
			++i;
			current = new Server(servs++);
			addServer(current);
			env = &(current->getEnvMap());
			while (brackets == 1 && i != tokens.end())
			{
				if (i->content == LOCATION)
				{
					current_loc = new location_t;
					env_loc = &current_loc->stuff;
					fill_line(env_loc, i);
					if (brackets++ > 1 || (i)->type != '{')
						throw Parsing::ErrorType();
					++i;
					for (; brackets == 2 && i != tokens.end(); ++i)
					{
						if (i->type == '}' && brackets-- && ++i != tokens.end())
							break ;
						fill_line(env_loc, i);
					}
					current->addLocation(current_loc);
					continue ;
				}
				// printf("called. (%i) \'%s\'\n", i->line_n, i->content.c_str());
				if (i->content == "}" && !--brackets)
					break ;
				fill_line(env, i);
				++i;
			}
			continue ;
		}
		fill_line(&_env, i);
	}
	for (serv_list::iterator i = _servers_down.begin(); i != _servers_down.end(); i++)
		(*i)->setup();
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
	// cout << "Waiting.\n";
		_sel.selectReadAndWrite();
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

// tries to setup all servers. If one fails it just keeps on building the others
void	Webserv::upAllServers()
{
	for (serv_list::iterator i = _servers_down.begin(); i != _servers_down.end() && _up;)
	{
		(*i)->printServerStats();
		try
		{
			(*i)->up();
			(*i)->_down_count = 0;
			_servers_up.push_front(*i);
			_servers_down.erase(i++);
		}
		catch(const std::exception& e)
		{
			if (++(*i)->_down_count >= DOWN_SERVER_TRIES_MAX)
				continue ;
			timestamp("Failed to setup Port " + itoa((*i)->getPort()) + ": " + string(e.what()) + '\n', ERROR);
			i++;
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


const string	&Webserv::getConf() const
{
	return (_conf);
}

//doesn't reload configuration
void	Webserv::setConf(string file_name)
{
	if (!_up)
		_conf = file_name;
}

