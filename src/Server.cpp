/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/29 15:08:38 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/09 21:36:49 by lpollini         ###   ########.fr       */
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

	_current_request.dir = msg.substr(space_pos + 1, msg.find(' ', space_pos + 1) - space_pos - 1);

	matchRequestLocation(_current_request);
	// truncate location identification part of dir
	_current_request.littel_parse(this);
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

//matches the request directory with a location and sets its location_t pointer
void	Server::matchRequestLocation(request_t &request) const
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
	}
}

status_code_t	Server::validateLocation()
{
	cout << "Looking for " << _current_request.dir << ". Allowed mwthod flag: " << (int)_current_request.loc->allows << "\n";
	_resp = _current_request;	// overloaded. Copies members dir and loc pointer
	string target_file;
	status_code_t	t;

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



bool Server::respond(int fd)
{
	std::cout << "Server " + itoa(_id) + ": Called by fd " << fd << " for response!\n";

	// FIND correct location
	// Set Responser's location
	// Launch Responser buildBody and buildHeader
	// Send response
	
	_resp.keepalive = true;
	_resp._res_code = validateLocation();
	cout << "Response code: " << _resp._res_code << '\n';
	if (_resp._res_code == _REQUEST_DIR_LISTING)
	// 	autoindexManager();
	//					To be implemented...
		_resp.getDir() = "";
	_resp.buildResponseBody();
	_resp.buildResponseHeader();
	_resp.Send(fd);
	_resp.clear();

	return _resp.keepalive;
}

// look for better method. Big switch uglyyy :((
void Responser::buildResponseBody()
{
	//check file existance
	string file = "";

	try
	{
		switch (_res_code)
		{
		case OK :
			cout << "Reading \'" << _dir << "\'\n";
		break ;
		case NOT_FOUND :
			_dir = _serv->getEnv(E_404, _loc);
		break ;
		case FORBIDDEN :
			_dir = _serv->getEnv(E_403, _loc);
		break ;
		case METHOD_NOT_ALLOWED :
			_dir = _serv->getEnv(E_405, _loc);
			cout << "Looking for 405 response in '" << file <<  "'\n";
		break ;
		default:
			timestamp("Could not send file at path \'" + _dir + "\'. Res code: " + itoa(_res_code) + '\n', ERROR);
			return ;
		}
		if (file.size())
			file = _serv->getEnv(LOC_ROOT) + '/' + file.substr(0, file.find(' '));
		_body = Parsing::read_file(_dir);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		cout << "Body not constructed! ):\n";
	}
	
}
