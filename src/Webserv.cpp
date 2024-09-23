/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:36 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/23 19:17:24 by lpollini         ###   ########.fr       */
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

/*//reads size bytes into read_buff which is assigned to dest
//returns the result of the read call
//on failure returns -1, sets dest to NULL and possibly shuts down the server
int	Webserv::socketRead(int fd, char **dest, size_t size)
{
	char *read_buff = _Singleton.read_buff; 
	int	bytes_read;
	static int read_count = 0;

	size = std::min(size, static_cast<size_t>(BUFFER_SIZE));

	bytes_read = read(fd, read_buff, size);

	if (bytes_read < 0)
	{
		timestamp("Failed read at fd: " + itoa(fd), ERROR); 
		if (fcntl(fd, F_GETFD) == -1)
		{
			timestamp(" The socket is closed!\n", ERROR, BOLD, false);
			timestamp("Shutting down server...\n", WARNING);
			_Singleton.downServer(fd);
		}
		else
			std::cout << std::endl;
		read_buff = NULL;
	}
	else
		read_buff[bytes_read] = 0;
	if (read_count++ == 4)
		close(fd);
	if (dest)
		*dest = read_buff;
	return (bytes_read);
}*/


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
			_servers_down.remove(*i++);
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


void	Webserv::downServer(Server *serv)
{
	serv->down();
	_servers_up.remove(serv);
	if (std::find(_servers_down.begin(), _servers_down.end(), serv) == _servers_down.end()) //if server is not in down list
	{
		_servers_down.push_front(serv);
	}
}

void Webserv::downServer(int fd)
{
	downServer(_sel._servs_map[fd]);
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

void	Webserv::reviveServers(ulong retry_time)
{
	for (serv_list::iterator it = _servers_down.begin(); it != _servers_down.end();)
	{
		try
		{
			if (!(*it)->tryUp(retry_time))
				continue ;
			_servers_up.push_front(*it);
			_sel.addConnectionServ((*it)->getSockFd(), *it);
			_servers_down.remove(*it++);
		}
		catch(const std::exception& e)
		{
			timestamp(string(e.what()) + "\n", ERROR);
			it++;
		}
	}

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
			reviveServers(SHORT_REVIVE_TIME);
			sleep(2);
			continue ;
		}
		cout << "Waiting.\n";
		_sel.selectReadAndWrite();
		usleep(20000);
		reviveServers();
		// sleep(2);
	}
	downAllServers();
	_up = false;
}