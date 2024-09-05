/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/05 17:57:40 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP

# define SERVER_HPP

# include "BetterSocket.hpp"
# include <stdio.h>
# include "definitions.hpp"
# include "Responser.hpp"
# include "useful_structs.hpp"

# define RECV_BUFF_SIZE 30000
# define HEAD_RESERVE 130

class Responser;

class	Server
{
	fd_list_t		_clientfds;

	short			_id;
	BetterSocket	_sock;
	char			_state;
	conf_t			_env;
	char			*_recieved_head;
	locations_list_t	_loc_ls;
	int				_fd;
	

	request_t		_current_request;
	Responser		_resp;

	string			_full_request_dir;


	Server&	operator=(const Server &assignment) {(void)assignment; return *this;}
	Server(const Server &copy) : _resp(this) {(void)copy;}
public:
	int						_down_count;
	struct __return_info	_return_info;
	bool					_is_sharing_port;
	static location_t		default_loc;

	Server(short id);
	~Server();

	int		Accept();
	void	addLocation(location_t *l);
	//returns environment variable given key
	string	&getEnv(string key, location_t *location = NULL);
	conf_t	&getEnvMap() {return _env;}
	int		&getSockFd() {return _sock.fd;}
	string	serverGetEnv(string key) const {return _env.at(key);}
	int		getPort() {return atoi(_env[PORT].c_str());}
	char	getState() {return _state;}
	int		getId() {return _id;}
	short	getRes() {return _resp._res_code;}
	int		getFd() {return _fd;}
	req_t	getReqType() {return _current_request.type;}
	
	void	setup();
	
	void	tryup();
	void	up();
	void	down();
	void	manageReturn(string &s);
	req_t	recieve(int fd, char *msg);
	bool	respond(int fd);
	void	closeConnection(int fd);
	void	createResp();
	req_t	parseMsg();

	void	printHttpRequest(string &msg, int fd_from);
	void 	matchRequestLocation(request_t &request); 
	void	lookForPlaceholders();

	status_code_t	validateLocation();
	status_code_t	manageDir();
	
	void	printServerStats();
	
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