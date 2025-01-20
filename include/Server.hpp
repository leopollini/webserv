/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fedmarti <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 11:32:33 by lpollini          #+#    #+#             */
/*   Updated: 2024/10/08 19:45:11 by fedmarti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP

# define SERVER_HPP

# include "BetterSocket.hpp"
# include <stdio.h>
# include "definitions.hpp"
# include "Responser.hpp"
# include "useful_structs.hpp"
# include "transfer_encoding.hpp"

# define HEAD_BUFFER 3000
# define HEAD_RESERVE 130

class Responser;

class	Server
{
	fd_list_t		_clientfds;

	short			_id;
	BetterSocket	_sock;
	char			_state;
	conf_t			_env;
	string			_received_head;
	locations_list_t	_loc_ls;
	int				_fd;
	

	request_t		_current_request;
	Responser		_resp;

	string			_full_request_dir;
	time_t			_lastUpAttempt;


	Server&	operator=(const Server &assignment) {(void)assignment; return *this;}
	Server(const Server &copy) : _resp(this) {(void)copy;}

public:
	int						_down_count;
	struct __return_info	_return_info;
	char					_is_sharing_port;
	static location_t		default_loc;
	string					_query_str;

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
	string	&getReqBody() {return _current_request.body;}
	
	void	setup();
	
	bool	tryUp(time_t retry_time);
	void	up();
	void	down();
	void	manageReturn(string &s);
	req_t	receive(int fd, string &msg, string &body);
	bool	respond(int fd);
	void	closeConnection(int fd);
	void	createResp();
	req_t	parseMsg();

	void	printServerStats();
	void	printHttpRequest(string &msg, int fd_from);
	void	lookForPlaceholders();

	void	HttpRequestLog(string &request_line, int fd_from);

	request_t &getCurrentRequest(void) {return (_current_request);};

	void 	matchRequestLocation(request_t &request); 
	
	status_code_t	validateLocation();
	status_code_t	manageDir();
	
	void	postRequestManager();
	short	deleteRequestManager();

	void	extensionPrioritySort();
	
	
	class DuplicateServLocation : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Duplicate location";}
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
	class SharedPortOccupied : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Shared port is occupied";}
	};
	class SharedPortNotAllowed : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Port sharing not allowed. Change in src/definitions.hpp";}
	};
	
private:
	req_t	_receiveBody(request_t &request) throw (Server::BodyMsgTooLong);
};

#endif