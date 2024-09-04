/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/03 20:04:09 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP

# define SERVER_HPP

# include "BetterSocket.hpp"
# include <stdio.h>
# include "definitions.hpp"
# include "Responser.hpp"
# include "useful_structs.hpp"
# include "TransferDecoder.hpp"

# define HEAD_RESERVE 130

class Responser;

class	Server
{
	Responser		_resp;
	fd_list			_clientfds;
	short			_id;
	BetterSocket	_sock;
	char			_state;
	conf_t			_env;
	locations_list	_loc_ls;

	request_t		_current_request;
	time_t			_lastUpAttempt;

	Server&	operator=(const Server &assignment) {(void)assignment; return *this;}
	Server(const Server &copy) : _resp(this) {(void)copy;}
public:
	int					_down_count;

	Server(short id);
	~Server();

	int		Accept();
	void	addLocation(location_t *l);
	conf_t	&getEnvMap() {return _env;}
	int		getSockFd() {return _sock.fd;}

	//returns environment variable given key
	string	&getEnv(string key, location_t *location = NULL);
	string	serverGetEnv(string key) const {return _env.at(key);}
	int		getPort() {return atoi(_env[PORT].c_str());}
	char	getState() {return _state;}
	int		getId() {return _id;}
	
	void	setup();
	
	bool	tryUp(time_t retry_time);
	void	up();
	void	down();
	req_t	receive(int fd);
	bool	respond(int fd);
	void	closeConnection(int fd);
	// req_t	parseMsg(int fd);

	void	HttpRequestLog(string &request_line, int fd_from);

	void 	matchRequestLocation(request_t &request) const; 
	
	status_code_t	validateLocation();
	status_code_t	manageDir();
	
	void	printServerStats()
	{
		cout << "Server " << _id << ":\n";
		cout << "\tName \'" << _env[NAME] << "\'\n";
		cout << "\tPort " << _env[PORT] << "\n";
		cout << "\tRoot " << _env[LOC_ROOT] << '\n';
		cout << "\tlocations (" << _loc_ls.size() << ")\n";
		for (locations_list::iterator i = _loc_ls.begin(); i != _loc_ls.end(); i++)
		{
			cout << "\t\t dir " << (*i)->stuff[L_DIR] << " allowed: ";
			for (str_set::iterator a = (*i)->allowed_extensions.begin(); a != (*i)->allowed_extensions.end(); a++)
				cout << *a << " ";
			cout << "| " << (int)(*i)->allows;
			cout << '\n';
		}
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
	class EmptyLocationDir : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Detected location directive without trailing directory";}
	};
	class BodyMsgTooLong : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Content size exceeded allowed body size";}
	};
};

#endif