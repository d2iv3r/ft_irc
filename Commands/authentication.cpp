/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   authentication.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: efarhat <efarhat@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/19 19:44:17 by hachahbo          #+#    #+#             */
/*   Updated: 2024/04/21 23:29:25 by efarhat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../Includes/command.hpp"

void Command::password(Client &_client, std::vector<std::string> args, std::string _psswd)
{
	if (args.size() == 0)
	{
		std::string buff = ERR_NEEDMOREPARAMS("PASS", this->hostname, "");
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;

		return;
	}
	_client.setpasswd(args.at(0));
	if (_client.getpasswd().compare(_psswd))
	{
		_client.setpasswd("");
		std::string buff = ERR_PASSWDMISMATCH(_client.getNickName(), this->hostname);
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;

		return;
	}

	_client.setIsvalid(1);
}

int parseTheNickname(std::string Nickname)
{
	if (std::isdigit(Nickname.at(0)))
		return (0);
	if (Nickname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_") != std::string::npos)
		return (0);
	return (1);
}
void Command::nickName(Client &_client, std::vector<std::string> args, Clients &cls, std::string _psswd, Channels &channs)
{
	std::string buff;
	if (_client.getIsvalid() < 1)
	{
		buff = ERR_NOTREGISTERED(_client.getNickName(), this->hostname);
		if (send(_client.getCd(), buff.c_str(), buff.size(), 0) == -1)
			std::perror("send");
		std::cout << buff << std::endl;
		return;
	}
	if (!_client.getNickName().empty() && _client.getIsvalid() == 2)
	{
		_client.errTheValid();
	}
	if (_client.getpasswd().empty() || _client.getpasswd().compare(_psswd))
	{
		buff = ERR_PASSWDMISMATCH(_client.getNickName(), this->hostname);
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;

		return;
	}
	if (args.size() == 0 || !args.at(0).find(":"))
	{
		buff = ERR_NEEDMOREPARAMS("NICK", this->hostname, "");
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;

		return;
	}
	// if (!args.at(0).compare("KICKI") && args.size() == 1)
	// {
	// 	buff = "this nick name is already reserved \n\n";
	// 	send(_client.getCd(), buff.c_str(), buff.size(), 0);
	// 	std::cout << buff << std::endl;

	// 	return;
	// }
	// else if (!args.at(0).compare("KICKI") && !args.at(1).compare("BOT"))
	// {
	// 	if (flag_help != 0)
	// 		flag = 0;
	// 	flag_help++;
	// 	_client.setNickName(args.at(0));
	// 	_client.setIsvalid(_client.getIsvalid() + 1);
	// 	return;
	// }
	if (args.size() != 1)
	{
		buff = ERR_ERRONEUSNICKNAME(args.at(0), this->hostname);
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;
		return;
	}
	if (!parseTheNickname(args.at(0)))
	{
		std::string buff = ERR_ERRONEUSNICKNAME(args.at(0), this->hostname);
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;
		return;
	}
	if (!cls.searchNickNameClient(_client.getCd(), args.at(0)))
	{
		std::string buff = ERR_NICKNAMEINUSE(args.at(0), this->hostname);
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;
		flag = 1;
		return;
	}
	if (flag_help != 0)
		flag = 0;
	flag_help++;
	std::string oldnick = _client.getNickName();
	_client.setNickName(args.at(0));
	if(_client.isAuthent())
	{
		channs.updateNick(_client, cls, oldnick, args.at(0));
		std::string msg = NICKNAME_RPLY(oldnick, _client.getUserName(), _client.getHostId(), args.at(0));
		send(_client.getCd(), msg.data(), msg.size(), 0);
	}
	if (_client.getpasswd().compare(_psswd))
	{
		std::string buff = ERR_PASSWDMISMATCH(_client.getNickName(), this->hostname);
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;
		return;
	}
	_client.setIsvalid(_client.getIsvalid() + 1);
}

int parseTheUsername(std::string username, std::string realName)
{
	if (username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_") != std::string::npos)
	{
		std::cout << ":invalid username  try agian !\n"
				  << std::endl;
		return (0);
	}
	if (realName.find_first_not_of("abcdefghijklmnopqrstuvwxyz ") != std::string::npos)
	{
		std::cout << ":invalid realname  try agian !\n"
				  << std::endl;
		return (0);
	}
	return (1);
}
void Command::userName(Client &_client, std::vector<std::string> args, Clients &cls, std::string _psswd)
{
	(void)_psswd;
	if (_client.getpasswd().empty() || _client.getNickName().empty())
	{
		std::string buff = ERR_NOTREGISTERED(_client.getNickName(), this->hostname);
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;

		return;
	}
	if (_client.getIsvalid() < 2)
	{
		std::string buff = ERR_NONICKNAMEGIVEN(args.at(0), this->hostname);
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;
		return;
	}
	if ((args.size() < 4 || args.at(0).empty()) && _client.getUserName().empty())
	{
		std::string buff = ERR_NEEDMOREPARAMS("USER", this->hostname, "");
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;
		return;
	}
	if ((_client.getNickName().empty() || _client.getpasswd().empty()) && flag == 0)
	{
		std::string buff = ERR_NONICKNAMEGIVEN(args.at(0), this->hostname);
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;
		return;
	}
	if (_client.getUserName().empty() && !parseTheUsername(args.at(0), args.at(3)))
		return;
	if (!_client.getUserName().empty())
	{
		std::string buff = ERR_ALREADYREGISTERED(_client.getNickName(), this->hostname);
		send(_client.getCd(), buff.c_str(), buff.size(), 0);
		std::cout << buff << std::endl;

		return;
	}
	if (!cls.searchUserNameClient(args.at(0)))
	{
		std::cout << "the userName is already exist choose another one \n";

		return;
	}
	_client.setUserName(args.at(0), args.at(3));
	_client.setIsvalid(_client.getIsvalid() + 1);
	if (_client.getIsvalid() == 3)
		lock = 1;
}

