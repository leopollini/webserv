/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BetterSelect.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/30 11:47:44 by lpollini          #+#    #+#             */
/*   Updated: 2024/06/01 14:34:26 by lpollini         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BETTERSELECT_HPP
# define BETTERSELECT_HPP

# include "Server.hpp"
# include "BetterSocket.hpp"
# include "utils.hpp"

# define SEL_TIMEOUT {1, 0}

# define RECV_BUFF_SIZE 1000000


struct	BetterSelect
{
	fd_set				_read_pool;
	fd_set				_write_pool;
	short				_tot_size;
	connections_map_t	_servs_map;
	connections_map_t	_clis_map;
	timeout_fd_t		_timeout_map;
	fd_list_t			_del_lst;
	int					_current_connection_fd;
	char				_recv_buff[RECV_BUFF_SIZE + 1];
	fd_msg_map			_recv_msg;

	char				_long_req_flag;

	static port_servs_map_t	_used_ports;

	BetterSelect&	operator=(const BetterSelect &assignment) {(void)assignment; return *this;}
	BetterSelect(const BetterSelect &copy) {(void)copy;}
	BetterSelect();
	~BetterSelect();

	void			loadServFds(serv_list_t &ls);

	void			addListeningConnection(int fd, Server *s);
	void			delListeningConnection(int pfd);
	void			addResponseConnection(int fd, Server *s);
	void			delResponseConnection(int pfd);
	void			addConnectionServ(int fd, Server *s);
	void			delConnectionServ(int pfd);

	void			closeAllClis();

	req_t			readMsg(int fd, connections_map_t::iterator &i);
	Server			*findServByHostname(port_t	port, string Host);
	
	void			err_close_clis();
	void			selectReadAndWrite();
	
	int				getBiggestFd();

	void			closeTimedOut();
	void			rmFd(int fd, Server *s);
private:

	void			_handleRequestResponse(fd_set &readfds, fd_set &writefds);
	void			_acceptNewConnections(fd_set &read_fds);

	class HeadMsgTooLong : public std::exception
	{
	public:
		virtual const char	*what() const throw() {return "Received too long a request";}
	};
};

#endif