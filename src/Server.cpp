/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:08:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/09/26 15:11:14 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Server.hpp"
#include "../include/Responser.hpp"
#include "Webserv.hpp"

location_t	Server::default_loc;

void	Server::printServerStats()
{
	cout << "Server " << _id << ":\n";
	cout << "\tIs sharing port: " << (_is_sharing_port ? "yes\n" : "no\n");
	cout << "\tName(s) \'" << _env[NAME] << "\'\n";
	cout << "\tPort " << _env[PORT] << "\n";
	cout << "\tRoot " << _env[LOC_ROOT] << '\n';
	cout << "\tlocations (" << _loc_ls.size() << ")\n";
	for (locations_list_t::iterator i = _loc_ls.begin(); i != _loc_ls.end(); i++)
	{
		cout << "\t\t dir " << (*i)->stuff[L_DIR] << " allowed: ";
		for (str_set_t::iterator a = (*i)->allowed_extensions.begin(); a != (*i)->allowed_extensions.end(); a++)
			cout << *a << " ";
		cout << "| " << (int)(*i)->allows;
		cout << " | redirection: " << (getEnv(LOC_RETURN, *i).empty() ? "none" : getEnv(LOC_RETURN, *i));
		cout << '\n';
	}
}

req_t Server::parseMsg()
{
	string	msg(_received_head);
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
	else
		return INVALID;

	_current_request.uri = msg.substr(space_pos + 1, msg.find(' ', space_pos + 1) - space_pos - 1);
	for (size_t i = _current_request.uri.size() - 1; _current_request.uri[i] == '/' && i; --i)
		_current_request.uri.erase(i);
	_full_request_dir = _current_request.uri;
	matchRequestLocation(_current_request);
	// truncate location identification part of dir
	_current_request.littel_parse(this);
	return _current_request.type;
}

static	size_t atoi2(string s)
{
	std::stringstream	str;
	size_t				res;

	if (s.empty())
		return -1;
	char	*end_of_string = const_cast<char *> (s.c_str() + s.size());
	return res = strtol(s.c_str(), &end_of_string, 10);
	str << std::dec << s;
	str >> res;
	return res;
}

void	Server::postRequestManager()
{
		const char *a = _current_request.body.c_str();
	if (_resp._res_code != 200)
		return ;
	if (_current_request.body.size() > atoi2(getEnv(MAX_BODY_SIZE, _current_request.loc)))
		return (void)(_resp._res_code = BAD_REQUEST);
	try
	{
		std::ofstream	file(_resp.getDir().c_str());
		file << _current_request.body << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		_resp._res_code = INTERNAL_SEVER_ERROR;
		return ;
	}
	_resp._res_code = _POST_SUCCESS;			// for later use. Sends bach the success page
}

void	Server::createResp()
{
	// Set Responser's location
	// Launch Responser buildBody and buildHeader
	_resp.keepalive = true;
	_resp._res_code = validateLocation();
	if (_current_request.type == POST)
		postRequestManager();
	if (_resp.buildResponseBody())
		return ;
	SAY("Response code: " << _resp._res_code << ": " << Webserv::getInstance().badExplain(_resp._res_code) << '\n');
	_resp.buildResponseHeader();
}

status_code_t	Server::validateLocation()
{
	SAY("Looking for " << _current_request.uri);
	SAY(". Allowed method flag: " << (int)_current_request.loc->allows);
	SAY(". Allowed extensions: ");
	if (DEBUG_INFO)
		for (str_set_t::iterator i = _current_request.loc->allowed_extensions.begin(); i != _current_request.loc->allowed_extensions.end(); ++i)
			cout << *i << " ";
	SAY('\n');
	_resp = _current_request;	// overloaded. Copies members dir and loc pointer
	status_code_t	t;

	if (_resp._is_returning > 0)
		return lookForPlaceholders(), ((status_code_t)(_resp._res_code = _return_info.code));

	if (_current_request.uri.empty())
		return (INTERNAL_SEVER_ERROR);
		
	_resp.getFileFlags() = checkCharacteristics(_current_request.uri.c_str());

	if (!exists(_resp.getFileFlags()) && _current_request.type != POST)
		return NOT_FOUND;

	if (!(_resp.getLoc()->allows & _current_request.type))
		return (METHOD_NOT_ALLOWED);

	if (_current_request.type == DELETE)
		return _REQUEST_DELETE;

	if (_resp._is_returning < 0)
		return _CGI_RETURN;

	if ((_resp.getFileFlags() & C_DIR) && (t = manageDir()) != _ZERO) // is a directory
		return t;

	else if (!(_resp.getFileFlags() & C_READ) && _current_request.type != POST)
		return FORBIDDEN;
		
	if (isOkToSend(_resp.getFileFlags()) || _current_request.type == POST)
		return OK;

	return INTERNAL_SEVER_ERROR;
}

req_t Server::receive(int fd, string &msg, string &body)
{
	_fd = fd;
	_received_head = msg;
	_current_request.body = body;
	_resp._res_code = 200;
	if (parseMsg() == INVALID)
		return INVALID;
	createResp();
	return _current_request.type;
}

static bool location_isinvalid(const location_t *loc, string &req)
{
	int	i = loc->dir.size();

	if (req.compare(0, i, loc->dir))
		return true;
	if (loc->dir[i - 1] != '/' && ((req[i] != '/' && req[i])))
		return true;
	if (loc->allowed_extensions.size() && (req.find_last_of('.') == string::npos || !loc->allowed_extensions.count(req.substr(req.find_last_of('.')))))
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
		SAY("Redirection read invalid return code. Returning default\n");
		_return_info.code = DEFAULT_RETURN_RESCODE;
	}
	_return_info.dir = s.substr(s.find(' ') + 1);
	SAY("#### REDIR INFO: " << _return_info.code << " " << _return_info.dir << "###\n");
}

//matches the request directory with a location and sets its location_t pointer
void	Server::matchRequestLocation(request_t &request)
{
	size_t max_len = 0;
	location_t *location = NULL;
	
	for (locations_list_t::const_iterator it = _loc_ls.begin(); it != _loc_ls.end(); it++)
	{
		string	&dir = (*it)->dir;

		// printf("called. \'%s\' (%i)\n", dir.c_str(), dir.size());
		//if directory is more specific, or if it doesn't match
		if (dir.size() - 1 > request.uri.size() || (dir.size() <= max_len && !(*it)->allowed_extensions.size()))
			continue ;

		if (request.uri.find(dir) || location_isinvalid(*it, request.uri))
			continue ;

		location = *it;
		max_len = dir.size();
	}
	request.loc = location;

	if (!location)
	{
		location = request.loc = &Server::default_loc;
		SAY("No location found. Returning default location: " << location->dir << '\n');
	}

	SAY("Found location: " << location->dir << '\n');
	SAY("\tlocation's root: " << location->stuff[LOC_ROOT] << '\n');
	if (!getEnv(LOC_RETURN, location).empty())
	{
		SAY("Warning! Location is trying to redirect...\n");
		try
		{
			manageReturn(getEnv(LOC_RETURN, location));
		}
		catch(const std::exception& e)
		{
			SAY("Redirection failed: " << e.what() << '\n');
		}
		_resp._is_returning = 1;
	}
	if (!location->stuff[LOC_CGI_RETURN].empty())
		_resp._is_returning = -1;
}

void	Server::lookForPlaceholders()
{
	size_t	pos;
	
	if ((pos = _return_info.dir.find('~')) != string::npos)
	{
		_return_info.dir.erase(pos, 1);
		_return_info.dir.insert(pos, _resp.getDir());
		SAY("#### New address after placeholder (~): " << _return_info.dir << std::endl);
	}
	else if ((pos = _return_info.dir.find('%')) != string::npos)
	{
		_return_info.dir.erase(pos, 1);
		_return_info.dir.insert(pos, _full_request_dir);
		SAY("#### New address after placeholder (%): " << _return_info.dir << std::endl);
	}
}

bool	Server::respond(int fd)
{
	SAY("Server " + itoa(_id) + ": Called by fd " << fd << " for response!\n");

	_resp.Send(fd);
	_resp.clear();

	return _resp.keepalive;
}
