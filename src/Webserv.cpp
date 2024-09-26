/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:36 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/26 18:51:36 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"
#include "parsing_util.hpp"

char	Webserv::_up = false;
Webserv Webserv::_Singleton;

Webserv &Webserv::getInstance()
{
	return (_Singleton);
}

Webserv::Webserv() : _conf(DEFAULT_CONF), _cgi_man(_sel)
{
	timestamp("Setting up Webserv!\n", CYAN);
	signal(SIGINT, gracefullyQuit);
	docTypesInit();

	Server::default_loc.allows = DEFAULT_LOCATION_ALLOWS;
}

Webserv::~Webserv()
{
	timestamp("Destroying Webserv!\n", BLUE);
	// for (serv_list_t::iterator i = _servers_up.begin(); i != _servers_up.end(); i++)
	// 	delete *i;
	for (serv_list_t::iterator i = _servers_down.begin(); i != _servers_down.end(); i++)
		delete *i;
}

#define BAD_FILL(x, y) _bad_explain[(x)] = (y)
#define DOCTYPE_FILE(x, y) _doc_types[(x)] = (y)
#define ENV_FILL(x, y) _env[x] = (y)

#define T_JOIN(x) string(DEFAULT_ERRPGS_DIR) + "/" + x + ".html"


// add anything useful. Every not recgnized extension is mapped to "default", which tells the browser to download the file
void	Webserv::docTypesInit()
{
	ENV_FILL(CGI_AUTOINDEX_DIR, DEFAULT_AUTOINDEX_CGI_DIR);
	ENV_FILL(L_INDEX, DEFAULT_INDEX_FILE);
	ENV_FILL(CGI_DELETE_DIR, DEFAULT_DELETE_CGI);
	ENV_FILL(E_400, T_JOIN("400"));
	ENV_FILL(E_302, T_JOIN("302"));
	ENV_FILL(E_301, T_JOIN("301"));
	ENV_FILL(E_307, T_JOIN("307"));
	ENV_FILL(E_308, T_JOIN("308"));
	ENV_FILL(E_403, T_JOIN("403"));
	ENV_FILL(E_404, T_JOIN("404"));
	ENV_FILL(E_405, T_JOIN("405"));
	ENV_FILL(E_500, T_JOIN("500"));

	DOCTYPE_FILE(".html", "text/html");
	DOCTYPE_FILE(".css", "text/css");
	DOCTYPE_FILE(".cpp", "text/cpp");
	DOCTYPE_FILE(".hpp", "text/hpp");
	
	BAD_FILL(1, "# cgi Autoindexing");
	BAD_FILL(-1, "# cgi generic");
	BAD_FILL(301, "Moved Permanently");
	BAD_FILL(302, "Found");
	BAD_FILL(307, "Temporary Redirect");
	BAD_FILL(308, "Permanent Redirect");
	BAD_FILL(400, "Bad Request");
	BAD_FILL(401, "Unauthorized");
	BAD_FILL(403, "Forbidden");
	BAD_FILL(404, "Not Found");
	BAD_FILL(405, "Method Not Allowed");
	BAD_FILL(418, "I'm a teapot");
	BAD_FILL(500, "Internal Server Error");

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
	for (serv_list_t::iterator i = _servers_down.begin(); i != _servers_down.end(); i++)
		(*i)->setup();
	// 	(*i)->locReadEnv();
	return 0;
}

void	Webserv::gracefullyQuit(int sig)
{
	(void)sig;

	timestamp("\b\bGracefully shutting Webserv! Send signal again to Force Close\n", GRAYI);
	signal(SIGINT, SIG_DFL);
	_up = 0;
}

void	Webserv::superPipeSet(int super, string body)
{
	_sel._super_pipe = super;
	_sel._super_body = body;
	
	if (_sel._super_pipe)
		_sel.addConnectionServ(_sel._super_pipe, (Server *)1);	// DUMMY SERVER!! NEVER USE!!!
	else
		_sel.delConnectionServ(_sel._super_pipe);
}

// tries to setup all servers. If one fails it just keeps on building the others
void	Webserv::upAllServers()
{
	for (serv_list_t::iterator i = _servers_down.begin(); i != _servers_down.end() && _up;)
	{
		try
		{
			(*i)->up();
			if (DEBUG_INFO)
				(*i)->printServerStats();
			(*i)->_down_count = 0;
			_servers_up.push_front(*i);
			_servers_down.remove(*i++);
		}
		catch(const std::exception& e)
		{
			close((*i)->getSockFd());
			++(*i)->_down_count;
			timestamp("Failed to setup Port " + itoa((*i)->getPort()) + ": " + string(e.what()) + '\n', ERROR);
			i++;
		}
	}
	_sel.loadServFds(_servers_up);
}

void	Webserv::downAllServers()
{
	for (serv_list_t::iterator i = _servers_up.begin(); i != _servers_up.end(); ++i)
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
	for (serv_list_t::iterator it = _servers_down.begin(); it != _servers_down.end();)
	{
		try
		{
 			if (!(*it++)->tryUp(retry_time))
				continue ;
			--it;
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

void	Webserv::start(char **prog_envp)
{
	timestamp("Starting Webserv!\n",GREEN);
	_up = 1;
	upAllServers();
	_cgi_man.envSet(prog_envp);
	timestamp("CGI manager setup done!\n",GREEN);
	
	while (_up)
	{
		if (!_servers_up.size())
		{
			SAY("No servers up!\n");
			// reviveServers(SHORT_REVIVE_TIME);
			sleep(2);
			continue ;
		}
		if (_up != 1)
			_up = false;
	// cout << "Waiting.\n";
		_sel.selectReadAndWrite();
		usleep(2000);
		// reviveServers(SHORT_REVIVE_TIME);
	}
	downAllServers();
	_sel.closeAllClis();
	_up = 0;
}
