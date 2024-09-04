/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:08:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/04 10:01:39 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Responser.hpp"
#include "Webserv.hpp"

void	Server::printServerStats()
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

req_t Server::parseMsg()
{
	string	msg(_recieved_head);
	string	cmd;
	size_t	space_pos;

	msg = msg.substr(0, msg.find('\n'));
	printHttpRequest(msg, _fd);
	cmd = msg.substr(0, space_pos = msg.find(' '));

	_current_request.type = INVALID;
	if (space_pos == string::npos || msg.find('/') != space_pos + 1)
		return INVALID;
	//  FIND HOST!!!
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
	if (_resp.buildResponseBody())
		return ;
	_resp.buildResponseHeader();
}

status_code_t	Server::validateLocation()
{
	cout << "Looking for " << _current_request.dir;
	cout << ". Allowed method flag: " << (int)_current_request.loc->allows << "\n";
	_resp = _current_request;	// overloaded. Copies members dir and loc pointer
	string target_file;
	status_code_t	t;

	if (_resp._is_returning > 0)
		return lookForPlaceholders(), ((status_code_t)(_resp._res_code = _return_info.code));

	if (_resp._is_returning < 0)
		return _CGI_RETURN;
	
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

req_t Server::recieve(int fd)
{
	_fd = fd;
	cout << "Readimg from " << _fd << "...\n";
	if (!(_msg_len = read(_fd, _recieved_head, HEAD_BUFFER)))
		return FINISH;
	if (_msg_len == HEAD_BUFFER)
		throw HeadMsgTooLong();
	_recieved_head[_msg_len] = 0;
	if (parseMsg() == INVALID)
		return INVALID;
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
			_resp._is_returning = 1;
		}
		if (!location->stuff[LOC_CGI_RETURN].empty())
			_resp._is_returning = -1;
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

bool	Server::respond(int fd)
{
	std::cout << "Server " + itoa(_id) + ": Called by fd " << fd << " for response!\n";

	// Send response
	
	_resp.Send(fd);
	_resp.clear();

	return _resp.keepalive;
}
