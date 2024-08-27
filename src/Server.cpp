/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:08:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/08/27 20:12:42 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Responser.hpp"
#include "Webserv.hpp"

req_t Server::parseMsg(int fd)
{
	string	msg(_recieved_head);
	string	cmd;
	size_t	space_pos;

	msg = msg.substr(0, msg.find('\n'));
	printHttpRequest(msg, fd);
	cmd = msg.substr(0, space_pos = msg.find(' '));

	if (space_pos > msg.size())
		return INVALID;
	//  FIND HOST!!!
	_current_request.type = INVALID;
	if (cmd == "GET")
		_current_request.type = GET;
	else if (cmd == "POST")
		_current_request.type = POST;
	else if (cmd == "DELETE")
		_current_request.type = DELETE;
	else if (cmd == "HEAD")
		_current_request.type = HEAD;
	// else
	// 	throw RequestNotHandled();

	_current_request.dir = msg.substr(space_pos + 1, msg.find(' ', space_pos + 1) - space_pos - 1);
	_full_request_dir = _current_request.dir;
	matchRequestLocation(_current_request);
	// truncate location identification part of dir
	_current_request.littel_parse(this);
	return _current_request.type;
}

void	Server::createResp()
{
	// Set Responser's location
	// Launch Responser buildBody and buildHeader
	_resp.keepalive = true;
	_resp._res_code = validateLocation();
	cout << "Response code: " << _resp._res_code << '\n';
	if (_resp._res_code == _REQUEST_DIR_LISTING)
	// 	autoindexManager();
	//					To be implemented...
		_resp.getDir() = "";
	_resp.buildResponseBody();
	_resp.buildResponseHeader();
}

req_t Server::recieve(int fd)
{
	cout << "Readimg from " << fd << "...\n";
	if (!(_msg_len = read(fd, _recieved_head, HEAD_BUFFER)))
		return FINISH;
	if (_msg_len == HEAD_BUFFER)
		throw HeadMsgTooLong();
	_recieved_head[_msg_len] = 0;
	parseMsg(fd);
	createResp();
	return _current_request.type;
}

static bool location_isvalid(const location_t *loc, string &req)
{
	int	i = loc->dir.size();

	if (req.compare(0, i, loc->dir))
		return true;
	if (loc->dir[i - 1] != '/' && ((req[i] != '/' && req[i])))
		return true;
	if (loc->allowed_extensions.size() && (req.find('.') == string::npos || !loc->allowed_extensions.count(req.substr(req.find('.')))))
		return true;
	return false;
}

void	Server::manageReturn(string &s)
{
	try
	{
		_return_info.code = atoi(s.c_str());
	}
	catch(const std::exception& e)
	{
		cout << "Redirection read invalid return code. Returning default\n";
		_return_info.code = DEFAULT_RETURN_RESCODE;
	}
	_return_info.dir = s.substr(s.find(' ') + 1);
	cout << "#### REDIR INFO: " << _return_info.code << " " << _return_info.dir << "###\n";
}

//matches the request directory with a location and sets its location_t pointer
void	Server::matchRequestLocation(request_t &request)
{
	if (request.type == INVALID)
		// *(int *)(0); //crash
		return ;

	size_t max_len = 0;
	location_t *location = NULL;
	
	for (locations_list::const_iterator it = _loc_ls.begin(); it != _loc_ls.end(); it++)
	{
		string	&dir = (*it)->dir;

		// printf("called. \'%s\' (%i)\n", dir.c_str(), dir.size());
		//if directory is more specific, or if it doesn't match
		if (dir.size() - 1 > request.dir.size() || dir.size() <= max_len)
			continue ;

		if (request.dir.find(dir) || location_isvalid(*it, request.dir))
			continue ;

		location = *it;
		max_len = dir.size();
	}
	request.loc = location;
	if (location)
	{
		cout << "Found location: " << location->dir << '\n';
		cout << "\tlocation's root: " << location->stuff[LOC_ROOT] << '\n';
		if (!getEnv(LOC_RETURN, location).empty())
		{
			cout << "Warning! Location is trying to redirect...\n";
			try
			{
				manageReturn(getEnv(LOC_RETURN, location));
			}
			catch(const std::exception& e)
			{
				cout << "Redirection failed: " << e.what() << '\n';
			}
			_resp._is_returning = true;
		}
	}
}

void	Server::lookForPlaceholders()
{
	size_t	pos;
	
	if ((pos = _return_info.dir.find('~')) != string::npos)
	{
		_return_info.dir.erase(pos, 1);
		_return_info.dir.insert(pos, _resp.getDir());
		cout << "#### New address after placeholder (~): " << _return_info.dir << std::endl;
	}
	else if ((pos = _return_info.dir.find('%')) != string::npos)
	{
		_return_info.dir.erase(pos, 1);
		_return_info.dir.insert(pos, _full_request_dir);
		cout << "#### New address after placeholder (%): " << _return_info.dir << std::endl;
	}
}

status_code_t	Server::validateLocation()
{
	cout << "Looking for " << _current_request.dir << ". Allowed method flag: " << (int)_current_request.loc->allows << "\n";
	_resp = _current_request;	// overloaded. Copies members dir and loc pointer
	string target_file;
	status_code_t	t;


	if (_resp._is_returning)
	{
		lookForPlaceholders();
		return ((status_code_t)(_resp._res_code = _return_info.code));
	}
	
	if (_current_request.dir.empty())
		return (INTERNAL_SEVER_ERROR);
		
	_resp.getFileFlags() = checkCharacteristics(_current_request.dir.c_str());

	if (!exists(_resp.getFileFlags()))
		return NOT_FOUND;
		
	if ((_resp.getFileFlags() & C_DIR) && (t = manageDir()) != _ZERO)// is a directory
		return t;

	if (!(_resp.getLoc()->allows & _current_request.type))
		return (METHOD_NOT_ALLOWED);
		
		
	if (isOkToSend(_resp.getFileFlags()))
		return OK;
		
	else if (_resp.getFileFlags() & C_FILE)
		return FORBIDDEN;

	return INTERNAL_SEVER_ERROR;
}


bool	Server::respond(int fd)
{
	std::cout << "Server " + itoa(_id) + ": Called by fd " << fd << " for response!\n";

	// Send response
	
	_resp.Send(fd);
	_resp.clear();

	return _resp.keepalive;
}

// look for better method. Big switch uglyyy :((
void Responser::buildResponseBody()
{
	//check file existance
	try
	{
		switch (_res_code)
		{
		case OK :
			cout << "Reading \n";
		 break ;
		case NOT_FOUND :
			_dir = _serv->getEnv(E_404, _loc);
			cout << "Looking for 404 response" <<  "'\n";
		 break ;
		case FORBIDDEN :
			_dir = _serv->getEnv(E_403, _loc);
			cout << "Looking for 403 response in \n";
		 break ;
		case METHOD_NOT_ALLOWED :
			_dir = _serv->getEnv(E_405, _loc);
			cout << "Looking for 405 response in \n";
		 break ;
		case MOVED_PERMANENTLY :
			_extra_args["Location"] = _serv->_return_info.dir;
			_dir = DEFAULT_MOVED_FILE;
			cout << "Preparing 301 response. Redir: " << _serv->_return_info.dir << "\n";
		 break ;
		case FOUND :
			_extra_args["Location"] = _serv->_return_info.dir;
			_dir = DEFAULT_MOVED_FILE;
			cout << "Preparing 302 response. Redir: " << _serv->_return_info.dir << "\n";
		 break ;
		case PERMANENT_REDIRECT :
			cout << "Preparing 308 response. New URL: " << _serv->_return_info.dir << "\n";
			_dir = DEFAULT_MOVED_FILE;
			_body = REDIR_URL(_serv->_return_info.dir);
		 return ;
		case TEMPORARY_REDIRECT :
			cout << "Preparing 307 response. New URL: " << _serv->_return_info.dir << "\n";
			_dir = DEFAULT_MOVED_FILE;
			_body = REDIR_URL(_serv->_return_info.dir);
		 return ;
		default:
			timestamp("Could not send file at path \'" + _dir + "\'. Res code: " + itoa(_res_code) + '\n', ERROR);
			return ;
		}
		_body = Parsing::read_file(_dir);
		
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		cout << "Body not constructed! ):\n";
	}
	
}
