/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efarhat <efarhat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 13:04:55 by efarhat           #+#    #+#             */
/*   Updated: 2024/04/22 17:01:54 by efarhat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <cerrno>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <exception>
#include <cstdlib>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <map>
#include <sstream>
#include <utility>
#include "clients.hpp"
#include "command.hpp"
#include "channels.hpp"
#include <fcntl.h>

#define MAX_LISTEN 6

class Server
{
	private:
		std::string			_passw;
		std::string			_port;
		int					_portt;
		int					_sd;
		std::vector<pollfd>	_pfds;
		Channels			_channels;
		Clients				_clients;

		void setNewClient(int cd, std::string id);
	public:
		Server();
		Server(char *po, char *pass);
		~Server();

		void start();
		void getSock();
};

#endif
