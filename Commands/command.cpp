#include "../Includes/command.hpp"
#include <unistd.h>
std::string Command::_cmdsList[];

void Command::init()
{
	std::string tmp[] = {"PASS", "NICK", "USER", "PRIVMSG", "JOIN", "KICK", "INVITE", "TOPIC", "MODE", "QUIT", "PONG"};
	for (size_t i = 0; i < NUMCMDS; i++)
		_cmdsList[i] = tmp[i];
}

std::string toLower(std::string str)
{
	std::string tmp = str;
	for (size_t i = 0; i < tmp.size(); i++)
		tmp[i] = std::tolower(tmp[i]);
	return (tmp);
}

int Command::isCommand(std::string const &str) const
{
	std::string tmp;
	for (size_t i = 0; i < NUMCMDS; i++)
	{
		tmp = toLower(_cmdsList[i]);
		if (!_cmdsList[i].compare(str) || !tmp.compare(str))
		{
			return (i + 1);
		}
	}
	return (0);
}
std::vector<std::pair<std::string, std::vector<std::string> > > Command::get_cmd()
{
	return (_cmds);
}
// std::vector<std::pair<std::string, std::vector<std::string> > > Command::get_cmds() const
// {
// 	return (_cmds);
// }

bool Command::isQuited() const {
	return(_quit);
}

void Command::setNameSender(std::string sender)
{
	this->sender = sender;
}
void Command::setNameReceiver(std::string receiver)
{
	this->receiver = receiver;
}
Command::Command(std::string &buff, Client &executer) : _executer(executer), _quit(false)
{
	if (!buff.compare("\n") || !buff.compare("\r\n") || !buff.compare("\r"))
	{
		buff.clear();
		return;
	}
	std::string word;
	std::string cmd;
	init();
	size_t start = 0;
	size_t end = 0;
	try
	{
		while (true)
		{
			end = buff.find("\n", start);
			if (end == std::string::npos)
				break;
			if (start > end)
				break;
			cmd = buff.substr(start, end - start);
			start = end + 1;
			if (cmd.back() == '\r')
				cmd.pop_back();
			std::istringstream t(cmd);
			if (t.fail())
				break;
			std::pair<std::string, std::vector<std::string> > p;
			t >> word;
			this->sender = word;
			p.first = word;
			while (t >> word)
			{
				if (word.find(":", 0) != std::string::npos)
				{
					std::string tmp;
					std::getline(t, tmp);
					word.erase(0, 1);
					if (!(word + tmp).empty())
						p.second.push_back(word + tmp);
					break;
				}
				p.second.push_back(word);
			}
			_cmds.push_back(p);
		}
	}
	catch (const std::exception &e)
	{
		buff.clear();
	}
	buff.clear();
}

Command::~Command()
{
}

void ftWelcome(Client executer, std::string hostname)
{
	std::cout << ":NICK " << executer.getNickName() << "\n\n";
	std::string buff = RPL_WELCOME(executer.getNickName(), hostname);
	send(executer.getCd(), buff.c_str(), buff.size(), 0);
	std::cout << buff << std::endl;
	buff = RPL_YOURHOST(executer.getNickName(), hostname);
	send(executer.getCd(), buff.c_str(), buff.size(), 0);
	std::cout << buff << std::endl;
	buff = RPL_CREATED(executer.getNickName(), hostname);
	send(executer.getCd(), buff.c_str(), buff.size(), 0);
	std::cout << buff << std::endl;
	buff = RPL_MYINFO(executer.getNickName(), hostname);
	send(executer.getCd(), buff.c_str(), buff.size(), 0);
	std::cout << buff << std::endl;
}
void Command::execute(Client &executer, Clients &cls, Channels &channs, std::string _psswd, std::vector<pollfd> &pfds)
{
	this->hostname = "FT_IRC";
	std::string rep;
	int sign = 0;
	for (size_t i = 0; i < _cmds.size(); i++)
	{
		int cmd = isCommand(_cmds[i].first);
		if (!executer.isAuthent() && (cmd != PASS && cmd != NICK && cmd != USER && cmd != 0 && cmd != QUIT))
		{
			rep = ERR_NOTREGISTERED(executer.getNickName(), this->hostname);
			if (send(executer.getCd(), rep.data(), rep.size(), 0) == -1)
				std::perror("send");
			break;
		}
		if ((cmd == PASS || cmd == USER) && executer.isAuthent())
		{
			rep = ERR_ALREADYREGISTERED(executer.getNickName(), this->hostname);
			if (send(executer.getCd(), rep.data(), rep.size(), 0) == -1)
				std::perror("send");
			break;
		}
		switch (cmd)
		{
		case 0:
			rep = ERR_UNKNOWNCOMMAND(this->hostname, executer.getNickName(), _cmds[i].first);
			if (send(executer.getCd(), rep.data(), rep.size(), 0) == -1)
				std::perror("send");
			break;
		case PASS:
			password(executer, _cmds[i].second, _psswd);
			break;
		case NICK:
			if(_cmds[i].second.size() == 1 && _cmds[i].second.at(0).compare(executer.getNickName()))
					sign = 1;
			nickName(executer, _cmds[i].second, cls, _psswd,  channs);
			break;
		case USER:
			userName(executer, _cmds[i].second, cls, _psswd);
			break;
		case JOIN:
			join(executer, _cmds[i].second, channs, cls);
			break;
		case PRIVMSG:
			prvMsg(executer, _cmds[i].second, channs, cls);
			break;
		case KICK:
			kick(executer, _cmds[i].second, channs, cls);
			break;
		case INVITE:
			invite(executer, _cmds[i].second, channs, cls);
			break;
		case TOPIC:
			topic(executer, _cmds[i].second, channs, cls);
			break;
		case MODE:
			mode(executer, _cmds[i].second, channs, cls);
			break;
		case QUIT:
			quit(executer, _cmds[i].second, cls, channs, pfds);
			break;
		default:
			break;
		}
		// if (executer.isAuthent() && flag == 0 && sign == 1)
		// {
		// 	// std::string buff = ":" + executer.getUserName() + " has changed his Nickname to " + executer.getNickName() + "\n\n";
		// 	// // std::string buff = NICKNAME_RPLY(executer.getNickName(), executer.getUserName(), executer.getHostId(), executer.)
		// 	// send(executer.getCd(), buff.c_str(), buff.size(), 0);
		// 	// std::cout << buff << std::endl;
		// 	flag = 1;
		// }
		if (executer.getIsvalid() == 3 && lock == 1)
		{
			executer.setAuthent(true);
			ftWelcome(executer, this->hostname);
			lock = 0;
		}
		// if (executer.getIsvalid() == 3 && lock == 1)
		// {
		// 	executer.setAuthent(true);
		// 	ftWelcome(executer, this->hostname);
		// 	lock = 0;
		// }
		// if (executer.getIsvalid() > 3 && flag == 0 && executer.getNickName().size() > 0 && executer.getUserName().size() > 0 && executer.getpasswd().size() > 0)
		// {
		// 	std::string buff = ":" + executer.getUserName() + " has changed his Nickname to " + executer.getNickName() + "\n\n";
		// 	send(executer.getCd(), buff.c_str(), buff.size(), 0);
		// 	std::cout << buff << std::endl;
		// 	flag = 1;
		// }
	}
}
