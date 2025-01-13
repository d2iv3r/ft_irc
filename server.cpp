#include "./Includes/server.hpp"

Server::Server()
{
}

Server::Server(char *po, char *pass)
{
	long p;
	char *end;

	p = std::strtol(po, &end, 10);
	if (*end)
		throw std::runtime_error("Error: Invaid port!");
	if (p < 1 || p > 65535)
	{
		std::cerr << "Error: port range not valid!" << std::endl;
		std::exit(1);
	}
	_port = po;
	_portt = p;
	_passw = pass;
	if (_passw.empty())
	{
		std::cerr << "Error: Invalid Password!" << std::endl;
		std::exit(1);
	}
}

Server::~Server()
{
}

void Server::setNewClient(int cd, std::string id)
{
	pollfd newfd;
	newfd.fd = cd;
	newfd.events = POLLIN;
	_pfds.push_back(newfd);
	_clients.addClient(cd, id);
}

void Server::start()
{
	getSock();
	struct pollfd pfd;
	struct sockaddr_in client;

	pfd.fd = _sd;
	pfd.events = POLLIN;
	_pfds.push_back(pfd);
	if (fcntl(_sd, F_SETFL, O_NONBLOCK) == -1)
		std::cerr << "Error: fcntl!" << std::endl;
	while (true)
	{
		int fds = poll(_pfds.data(), _pfds.size(), -1);
		if (fds == -1)
		{
			std::perror("poll");
			continue;
		}
		std::string str;
		int cfd;
		for (size_t i = 0; i < _pfds.size(); i++)
		{
			if (_pfds[i].revents & POLLIN)
			{
				if (_pfds[i].fd == _sd)
				{
					socklen_t addrlen = sizeof(client);
					while (true)
					{
						cfd = accept(_sd, (sockaddr *)&client, &addrlen);
						if (cfd == -1)
						{
							if (errno != EWOULDBLOCK)
								std::perror("accept");
							break;
						}
						else
						{
							str = inet_ntoa(client.sin_addr);
							setNewClient(cfd, str);
						}
					}
				}
				else
				{
					char buff[513];
					std::memset(buff, 0, 513);
					while (true)
					{
						int rdd = recv(_pfds[i].fd, buff, 512, 0);
						if (rdd == -1)
						{
							if (errno != EWOULDBLOCK) {
								std::perror("recv");
								rdd = 0;
							}
							break;
						}
						if (rdd == 0)
						{
							std::cout << "Client " << _pfds[i].fd << " disconnected!" << std::endl;
							_channels.quit(_clients[_pfds[i].fd], _clients);
							_channels.removeClient(_clients[_pfds[i].fd], _clients);
							_clients.eraseClient(_pfds[i].fd);
							close(_pfds[i].fd);
							_pfds.erase(_pfds.begin() + i);
							--i;
							break;
						}
						_clients[_pfds[i].fd]._buff.append(buff);
						if (_clients[_pfds[i].fd]._buff.find("\n") != std::string::npos)
						{
							Command cmd(_clients[_pfds[i].fd]._buff, _clients[_pfds[i].fd]);
							cmd.execute(_clients[_pfds[i].fd], _clients, _channels, _passw, _pfds);
							if (cmd.isQuited()) {
								_pfds.erase(_pfds.begin() + i);
								--i;
								break ;
							}
						}
					}
				}
			}
		}
	}
}

void Server::getSock()
{
	_sd = socket(PF_INET, SOCK_STREAM, 0);
	if (_sd == -1)
	{
		std::perror("Error: socket");
		std::exit(1);
	}
	int yes = 1;
	if (setsockopt(_sd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) == -1
		|| setsockopt(_sd, SOL_SOCKET, SO_NOSIGPIPE, &yes, sizeof(yes)) == -1)
	{
		close(_sd);
		std::perror("Error: setsockopt");
		std::exit(1);
	}
	sockaddr_in addrin;
	std::memset(&addrin, 0, sizeof(addrin));
	addrin.sin_family = AF_INET;
	addrin.sin_addr.s_addr = htonl(INADDR_ANY);
	addrin.sin_port = htons(_portt);
	if (bind(_sd, (sockaddr *)&addrin, sizeof(addrin)) == -1)
	{
		close(_sd);
		std::perror("Error: bind");
		std::exit(1);
	}
	if (listen(_sd, MAX_LISTEN) == -1)
	{
		close(_sd);
		std::perror("Error: listen");
		std::exit(1);
	}
}
