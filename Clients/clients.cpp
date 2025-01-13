#include "../Includes/clients.hpp"
#include "../Includes/replies.hpp"
#include <sys/types.h>
#include <sys/socket.h>
///////////// Client //////////

Client::Client()
{
	this->valid = 0;
	this->flag = 0;
	_quit = false;
	this->_auth = false;
}

Client::Client(int cd) : _cd(cd), _regi(false), _auth(false)
{
	this->valid = 0;
	this->flag = 0;
	_quit = false;
	this->_auth = false;
}

Client::Client(int cd, std::string nick) : _cd(cd), _regi(false), _auth(false), _nick_name(nick)
{
	this->valid = 0;
	this->flag = 0;
	_quit = false;
	this->_auth = false;
}

Client::Client(Client const &cl)
{
	*this = cl;
}

Client &Client::operator=(Client const &cl)
{
	if (this != &cl)
	{
		this->_cd = cl._cd;
		this->_nick_name = cl._nick_name;
		this->_host_name = cl._host_name;
		this->_auth = cl._auth;
	}
	return (*this);
}

Client::~Client()
{
}
//------------- setter ------------- //
void Client::setpasswd(std::string passwd)
{
	this->_password = passwd;
}
void Client::setNickName(std::string nickName)
{
	this->_nick_name = nickName;
}
void Client::setUserName(std::string UserName, std::string name)
{
	this->_user_name = UserName;
	this->_real_name = name;
}

void Client::setIsvalid(int valid)
{
	this->valid = valid;
}
void Client::errTheValid()
{
	if (valid - 1 < 0)
		valid = 0;
	else
		valid--;
}
void Client::setRegistred(bool var)
{
	this->_regi = var;
}
void Client::setAuthent(bool var)
{
	this->_auth = var;
}
void Client::setHostId(std::string ipAddr)
{
	this->_host_name = ipAddr;
}

//------------- getter ------------- //
bool Client::getauth() { return (this->_auth); }
std::string Client::getpasswd() { return (this->_password); }
std::string Client::getNickName() { return (this->_nick_name); }
std::string Client::getUserName() { return (this->_user_name); }
int Client::getIsvalid() { return (this->valid); }
std::string Client::getHostId() { return (this->_host_name); }
int Clients::getSizeOfClients()
{
	return (this->_clns.size());
}

// Client Client::getClient(std::string nick) {
// 	std::map<int, Client>::iterator it;
// 	for (it = _clns.begin(); it != _clns.end(); ++it)
// 	{
// 		if (it->second.getNickName() == nick)
// 		{
// 			return it->second;
// 		}
// 	}
// 	return NULL;
// }

// -------------------------------- //

bool Client::isRegistred() const
{
	return (_regi);
}

bool Client::isAuthent() const
{
	return (_auth);
}

int Client::getCd() const
{
	return (_cd);
}

std::string Client::getNick() const { return (_nick_name); }

void Client::setRegi(bool r)
{
	_regi = r;
}

// ------------------------------ //

///////// Clients /////////

Clients::Clients()
{
}

Clients::Clients(Clients const &cl)
{
	*this = cl;
}

Clients &Clients::operator=(Clients const &cl)
{
	if (this != &cl)
	{
	}
	return (*this);
}

Clients::~Clients()
{
}

bool Clients::isClient(std::string const &nick) {
	_clientsIt it;

	for (it = _clns.begin(); it != _clns.end(); it++)
	{
		if (it->second.getNickName() == nick)
			return (true);
	}
	return (false);
}

Client const Clients::getClient(std::string const &nick) {
	_clientsIt it;

	for (it = _clns.begin(); it != _clns.end(); it++)
	{
		if (it->second.getNickName() == nick)
			return (it->second);
	}
	return (0);
}

void Clients::addClient(int cd, std::string ipAdd)
{
	std::pair<int, Client> p;
	Client obj(cd);
	p.first = cd;
	// obj.setNickName("test");
	obj.setRegistred(true);
	obj.setHostId(ipAdd);
	obj._quit = false;
	p.second = obj;
	_clns.insert(p);
}

Client Clients::getClient(int cd)
{
	_it = _clns.find(cd);
	if (_it == _clns.end())
		throw NOTACLIENT;
	return (_it->second);
}

Client &Clients::operator[](int cd)
{
	_it = _clns.find(cd);
	return (_it->second);
}
void Clients::eraseClient(int cd)
{
	_it = _clns.find(cd);
	if (_it != _clns.end())
	{
		_clns.erase(_it);
	}
}

int Clients::searchNickNameClient(std::string _nickName)
{
	if (!_clns.size())
		return 1;
	for (_it = _clns.begin(); _it != _clns.end(); ++_it)
	{
		if (_it->second.getNickName() == _nickName)
		{
			return 0;
		}
	}
	return 1;
}
int Clients::searchNickNameClient(int fd,  std::string _nickName)
{
	if (!_clns.size())
		return 1;
	for (_it = _clns.begin(); _it != _clns.end(); ++_it)
	{
		if (_it->second.getNickName() == _nickName && _it->second.getCd() != fd)
		{
			return 0;
		}
	}
	return 1;
}
void Client::setCd(int fd)
{
	this->_cd = fd;
}

int Clients::searchUserNameClient(std::string _UserName)
{
	for (_it = _clns.begin(); _it != _clns.end(); ++_it)
	{
		if (_it->second.getUserName() == _UserName)
			return 0;
	}
	return 1;
}

Client &Clients::findTheReciever(int fd, std::string nick, std::string _nickName, std::string hostname)
{
	if (_clns.empty())
	{
		throw std::runtime_error("Client list is empty");
	}
	for (_it = _clns.begin(); _it != _clns.end(); ++_it)
	{
		if (_it->second.getNickName() == _nickName)
		{
			_it->second.setCd(_it->first);
			return _it->second;
		}
	}
	std::string buff = ERR_NOSUCHNICK(hostname, nick, _nickName);
	(void)fd;
	if (send(fd, buff.c_str(), buff.size(), 0) == -1)
		std::perror("send");
	std::cout << buff << std::endl;
	throw std::runtime_error("");
}
