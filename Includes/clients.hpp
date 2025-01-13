#ifndef CLIENTS_HPP
#define CLIENTS_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <map>
#include <utility>

#define NOTACLIENT 21

class Client
{
	private:
		int			_cd;
		bool		_regi;
		bool		_auth;
		std::string _password;
		std::string _user_name;
		std::string	_nick_name;
		std::string	_host_name;
		std::string	_server_name;
		std::string	_real_name;
		int 		valid;
		int flag;
	public:
		bool        _quit;
		std::string	_buff;
		Client();
		Client(int cd);
		Client(int cd, std::string nick);
		Client(Client const &cl);
		Client &operator=(Client const &cl);
		~Client();

		bool isRegistred() const;
		bool isAuthent() const;
		void setRegi(bool r);
		int getCd() const;
		std::string getNick() const;

		//----------- setter----------//
		void setpasswd(std::string passwd);
		void setCd(int fd);
		void setNickName(std::string nickName);
		void setUserName(std::string UserName, std::string name);
		void setIsvalid(int valid);
		void setRegistred(bool var);
		void setAuthent(bool var);
		void setHostId(std::string ipAddr);
		void errTheValid();

		//----------- getter----------//

		std::string getpasswd();
		std::string getNickName();
		std::string getUserName();
		std::string getHostId();
		bool getauth();
		int getIsvalid();
};

class Clients
{
	private:
		std::map<int, Client>::iterator	_it;
		typedef std::map<int, Client>::iterator _clientsIt;
		std::map<int, Client> _clns;

	public:
		Clients();
		Clients(Clients const &cls);
		Clients &operator=(Clients const &cls);
		~Clients();

		void addClient(int cd, std::string ipAdd);

		const Client getClient(std::string const &nick);
		bool isClient(std::string const &nick);
		Client getClient(int cd);
		Client &operator[](int  cd);

		void eraseClient(int cd);
		int getSizeOfClients();
		int searchNickNameClient(std::string _nickName);
		int searchNickNameClient(int fd,  std::string _nickName);
		int searchUserNameClient(std::string _nickName);
		// Client & findTheReciever(std::string _nickName);
		Client & findTheReciever(int fd, std::string nick, std::string _nickName, std::string hostname);
};

#endif
