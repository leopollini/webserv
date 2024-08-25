/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/08/25 20:24:24 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP

# define SERVER_HPP

# include "BetterSocket.hpp"
# include <stdio.h>
# include "definitions.hpp"
# include "Responser.hpp"
# include "useful_structs.hpp"

# define HEAD_BUFFER 3000
# define HEAD_RESERVE 130

class Responser;

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

	string			_full_request_dir;


	Server&	operator=(const Server &assignment) {(void)assignment; return *this;}
	Server(const Server &copy) : _resp(this) {(void)copy;}

public:
	int						_down_count;
	struct __return_info	_return_info;

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
	
	void	tryup();
	void	up();
	void	down();
	void	manageReturn(string &s);
	req_t	recieve(int fd);
	bool	respond(int fd);
	void	closeConnection(int fd);
	req_t	parseMsg(int fd);

	void	printHttpRequest(string &msg, int fd_from);


	void 	matchRequestLocation(request_t &request); 
	
	void	lookForPlaceholders();

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
			cout << " | redirection: " << (getEnv(LOC_RETURN, *i).empty() ? "none" : getEnv(LOC_RETURN, *i));
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
};

#endif