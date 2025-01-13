#include "../Includes/bot.hpp"

Bot::Bot()
{
}

Bot::Bot(std::string const &host, std::string const &port, std::string const &pass, std::string const &nick) : _host(host), _pass(pass)
{
	long p;
	char *end;

	p = std::strtol(port.c_str(), &end, 10);
	if (*end)
		throw std::runtime_error("Error: Invalid port!");
	_port = p;
	if (_pass.empty())
		throw std::runtime_error("Error: Invalid password!");
	std::ifstream dataFile("./Bot/data.txt");
	if (!dataFile.is_open())
		throw std::ifstream::failure("Error: opening the data file!");
	std::string line;
	while (std::getline(dataFile, line))
		_data.push_back(line);
	dataFile.close();
	if (nick.empty() || nick.size() > 15)
		throw std::runtime_error("Error: Select a valid nick name for the bot!");
	_nickName = nick;
}

void Bot::regist()
{
	std::string reg = REGISTER(_nickName, _pass);
	if (send(_sock, reg.data(), reg.size(), 0) == -1) {
		std::cerr << "Error: can't send to server!" << std::endl;
		std::exit(1);
	}
	char buff[513];
	std::memset(buff, 0, 513);
	int rec = recv(_sock, buff, 512, 0);
	if (rec == -1) {
		std::cerr << "Error: recv!" << std::endl;
		std::exit(1);
	}
	if (rec == 0) {
		std::cerr << "Server is down!" << std::endl;
		close(_sock);
		std::exit(1);
	}
	std::string str = buff;
	if (str.find("464") != std::string::npos) {
		std::cerr << "Error: server password incorrect" << std::endl;
		close(_sock);
		std::exit(1);
	}
	else if (str.find("432") != std::string::npos) {
		std::cerr << "Error: select a valid nick name for the bot!" << std::endl;
		close(_sock);
		std::exit(1);
	}
	else if (str.find("433") != std::string::npos) {
		std::cerr << "Error: the nick name is already used  select another one!" << std::endl;
		close(_sock);
		std::exit(1);
	}
}

bool Bot::isClientPrivmsg(std::string buff)
{
	size_t pos = buff.find("PRIVMSG " + _nickName);
	if (pos != std::string::npos) {
		size_t endNick = buff.find("!~");
		pos = buff.find(":", pos);
		if (endNick == std::string::npos || pos == std::string::npos)
			return (false);
		_nick = buff.substr(1, endNick - 1);
		_message = buff.substr(pos + 1);
		if (_message.back() == '\n')
		{
			_message.pop_back();
			if (_message.back() == '\r')
				_message.pop_back();
		}
		return (true);
	}
	return (false);
}

bool Bot::isSwear(std::string message)
{
	for (size_t i = 0; i < _data.size(); i++)
	{
		if (message.find(_data[i]) != std::string::npos)
			return (true);
	}
	return (false);
}

bool Bot::isInvite(std::string buff)
{
	size_t pos = buff.find("INVITE");
	if (pos != std::string::npos)
	{
		size_t endNick = buff.find("!~");
		pos = buff.find("#", pos);
		if (endNick == std::string::npos || pos == std::string::npos)
			return (false);
		_nick = buff.substr(1, endNick - 1);
		_channel = buff.substr(pos);
		if (_message.back() == '\n')
		{
			_message.pop_back();
			if (_message.back() == '\r')
				_message.pop_back();
		}
		return (true);
	}
	return (false);
}

bool Bot::isChannelPrivmsg(std::string buff)
{
	size_t pos = buff.find("PRIVMSG #");
	if (pos != std::string::npos)
	{
		size_t endNick = buff.find("!~");
		pos = buff.find("#", pos);
		if (endNick == std::string::npos || pos == std::string::npos)
		{
			return (false);
		}
		_nick = buff.substr(1, endNick - 1);
		_channel.clear();
		while (pos < buff.size() && buff[pos] != ' ')
			_channel.push_back(buff[pos++]);
		pos = buff.find(":", pos);
		if (pos == std::string::npos)
			return (false);
		_message = buff.substr(pos + 1);
		if (_channel.back() == '\n')
		{
			_channel.pop_back();
			if (_message.back() == '\r')
				_message.pop_back();
		}
		return (true);
	}
	return (false);
}

void Bot::parseReq(std::string buff)
{
	std::string reply;
	if (isClientPrivmsg(buff))
	{
		std::cout << "I received a privmsg from " << _nick << " the message ==> " << _message << std::endl;
		std::cout << _message.compare("HELP") << std::endl;
		std::string s = "HELP";
		if (_message == s)
			reply = "PRIVMSG " + _nick + " :" + RPLY_HELP;
		else
			reply = "PRIVMSG " + _nick + " :" + RPLY_HELLO(_nick);
		if (send(_sock, reply.data(), reply.size(), 0) == -1)
		{
			std::cerr << "Error: send fails!" << std::endl;
			return;
		}
	}
	else if (isInvite(buff))
	{
		std::cout << "I reeceived a invite to channel " << _channel << "from " << _nick << std::endl;
		reply = "JOIN " + _channel + "\n";
		if (send(_sock, reply.data(), reply.size(), 0) == -1)
		{
			std::cerr << "Error: send fails!" << std::endl;
			return;
		}
		reply = "PRIVMSG " + _nick + " :" + RPLY_INVITATION(_nick, _channel);
		if (send(_sock, reply.data(), reply.size(), 0) == -1)
		{
			std::cerr << "Error: send fails!" << std::endl;
			return;
		}
		reply = "PRIVMSG " + _channel + " :" + RPLY_CHANNEL(_channel);
		if (send(_sock, reply.data(), reply.size(), 0) == -1)
		{
			std::cerr << "Error: send fails!" << std::endl;
			return;
		}
	}
	else if (isChannelPrivmsg(buff))
	{
		if (isSwear(_message))
		{
			reply = "KICK " + _channel + " " + _nick + " :No swearing in the channel!\n";
			if (send(_sock, reply.data(), reply.size(), 0) == -1)
			{
				std::cerr << "Error: send fails!" << std::endl;
				return;
			}
		}
	}
}

void Bot::listenMsg()
{
	char buff[513];
	while (true)
	{
		std::memset(buff, 0, 513);
		int rec = recv(_sock, buff, 512, 0);
		if (rec == -1)
		{
			std::cerr << "Error: recv!" << std::endl;
			std::exit(1);
		}
		if (rec == 0) {
			std::cerr << "Server is down!" << std::endl;
			std::exit(1);
		}
		else
		{
			std::cout << "Received ==> " << buff << std::endl;
			parseReq(buff);
		}
	}
}

void Bot::start()
{
	sockaddr_in addr;
	_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (_sock == -1)
		throw std::runtime_error("Error: creating the socket!");
	std::memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	if (inet_pton(AF_INET, _host.c_str(), &addr.sin_addr) == -1)
		throw std::runtime_error("Error: Invalid host address!");
	if (connect(_sock, (sockaddr *)&addr, sizeof(addr)) == -1)
	{
		std::perror("connect");
		throw std::runtime_error("Error: connection failed!");
	}
	regist();
	listenMsg();
}

int main(int argc, char *argv[])
{
	if (argc != 5)
	{
		std::cout << "Error: Invalid arguments!\n<./bot> <hostname> <port> <password> <nick_name>\n";
		return (1);
	}
	try
	{
		Bot bot(argv[1], argv[2], argv[3], argv[4]);
		bot.start();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}
