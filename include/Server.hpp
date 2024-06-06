/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/06 19:22:10 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP

# define SERVER_HPP

# include "BetterSocket.hpp"
# include <stdio.h>
# include "definitions.hpp"

# define HEAD_BUFFER 3000

# define NEW_SERVER "server"
# define NAME "server_name"
# define PORT "listen"
# define LOCATION "location"
# define LOC_ROOT "root"
# define L_DIR "location"

struct request_t
{
	req_t		type;
	string		dir;
	string		host;
	location_t	*loc;
};

struct response_t
{
	string		head;
	string		body;
	string		dir;
	location_t	*loc;

	void	Clear()
	{
		head.clear();
		body.clear();
		dir.clear();
	}
	size_t	Size()
	{
		return head.size() + body.size();
	}
};

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
	response_t		_current_response;
	short			_res_code;

	Server&	operator=(const Server &assignment) {(void)assignment; return *this;}
	Server(const Server &copy) {(void)copy;}
public:
	int					_down_count;
	conf_t				_sconf;
	

	// Server(port_t port);
	Server(port_t port, string name);
	Server(short id);
	~Server();

	conf_t	*getEnv() {return &_env;}
	int		getSockFd();
	int		getPort() {return atoi(_env[PORT].c_str());}
	char	getState() {return _state;}
	int		Accept();
	int		getId() {return _id;}
	void	addLocation(location_t *l) {_loc_ls.push_back(l);}

	bool	tryup();
	void	up();
	void	down();
	req_t	recieve(int fd);
	void	respond(int fd);
	void	closeConnection(int fd);
	req_t	parseMsg(int fd);

	void	printHttpRequest(string &msg, int fd_from);

	void	buildResponseHeader();
	string	badExplain(short code) {return itoa(code);}
	string	getDocType() {return "default";}
	size_t	getResLen() {return -1000;}
	string	getResServer() {return "Lolserv";}

	void 	matchRequestLocation(request_t &request);

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
};

#endif