/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/08 20:08:32 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP

# define SERVER_HPP

# include "BetterSocket.hpp"
# include <stdio.h>
# include "definitions.hpp"
# include "Responser.hpp"

# define HEAD_BUFFER 3000

# define NEW_SERVER "server"
# define NAME "server_name"
# define PORT "listen"
# define LOCATION "location"
# define LOC_ROOT "root"
# define L_DIR "location"
# define L_INDEX "index"
# define L_DIR_LISTING "autoindex"

class Responser;

struct	location_t
{
	conf_t	stuff;
	string	dir;
	char	allows;
	string	res_403_dir;
	string	res_404_dir;
};

class	Server
{
	fd_list			_clientfds;

	short			_id;
	BetterSocket	_sock;
	char			_state;
	conf_t			_env;
	char			_recieved_head[HEAD_BUFFER];
	size_t			_msg_len;
	locations_list	_loc_ls;

	request_t		_current_request;
	Responser		_resp;

	Server&	operator=(const Server &assignment) {(void)assignment; return *this;}
	Server(const Server &copy) : _resp(this) {(void)copy;}
public:
	int					_down_count;

	Server(short id);
	~Server();

	int		getSockFd();
	int		Accept();
	void	addLocation(location_t *l);
	conf_t	&getEnvMap() {return _env;}

	//returns environment variable given key
	string	getEnv(string key, location_t *location = NULL) const;
	string	serverGetEnv(string key) const {return _env.at(key);}
	int		getPort() {return atoi(_env[PORT].c_str());}
	char	getState() {return _state;}
	int		getId() {return _id;}

	bool	tryup();
	void	up();
	void	down();
	req_t	recieve(int fd);
	void	respond(int fd);
	void	closeConnection(int fd);
	req_t	parseMsg(int fd);

	void	printHttpRequest(string &msg, int fd_from);


	void 	matchRequestLocation(request_t &request) const; 
	
	status_code_t	validateLocation();
	
	void	printServerStats()
	{
		cout << "Server " << _id << ":\n";
		cout << "\tName \'" << _env[NAME] << "\'\n";
		cout << "\tPort " << _env[PORT] << "\n";
		cout << "\tRoot " << _env[LOC_ROOT] << '\n';
		cout << "\tlocations (" << _loc_ls.size() << ")\n";
		for (locations_list::iterator i = _loc_ls.begin(); i != _loc_ls.end(); i++)
			cout << "\t\t dir " << (*i)->stuff[L_DIR] << '\n';
	}
	
	class HeadMsgTooLong : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Recieved too long a request";}
	};
	class DuplicateServLocation : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Parser tried to insert the same location twice";}
	};
};

#endif