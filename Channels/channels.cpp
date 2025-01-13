#include "../Includes/channels.hpp"

Channels::Channels() {
}

Channels::Channels(Channels const &chs) {
	*this = chs;
}

Channels &Channels::operator=(Channels const &chs) {
	if (this != &chs) {
		_channs = chs._channs;
	}
	return (*this);
}

Channels::~Channels() {
}

bool Channels::isChannel(std::string const &name) {
	_channsIt	it;
	it = _channs.find(name);
	if (it == _channs.end())
		return (false);
	return (true);
}

void Channels::addChannel(std::string const &name, std::string const &ope) {
	_channs[name] = Channel(name, ope);
}

void Channels::removeChann(std::string const &name) {
	if (_channs.find(name) != _channs.end())
		_channs.erase(name);
}

Channel &Channels::operator[](std::string const &ch) {
	_channsIt	it;
	it = _channs.find(ch);
	return (it->second);
}

void Channels::removeClient(Client &cl, Clients &cls)
{
	_channsIt	it;
	(void)cls;
	// std::string reply;
	// reply = ":" + cl.getNickName() + "!~" + cl.getUserName() + "@" + cl.getHostId() + " QUIT :Quit: Leaving...\r\n";
	for (it = _channs.begin(); it != _channs.end(); it++)
	{
		if (it->second.isMember(cl.getNickName())) {
			// it->second.sendToMembers(reply, cls);
			it->second.removeMember(cl.getNickName());
		}
	}
	it = _channs.begin();
	while (it != _channs.end())
	{
		if (it->second.getSizeMembers() == 0)
		{
			it = _channs.erase(it);
		}
		else
			++it;
	}
}

void Channels::updateNick(Client &executer, Clients &cls, std::string const &old_nick, std::string const &new_nick)
{
	_channsIt	it;
	for (it = _channs.begin(); it != _channs.end(); it++)
	{
		it->second.updateNick(executer, cls, old_nick, new_nick);
	}
}

void Channels::quit(Client &executer, Clients &cls) {
	_channsIt	it;

	std::string reply = RPL_QUIT(executer.getNickName(), executer.getHostId(), "");
	for (it = _channs.begin(); it != _channs.end(); it++)
	{
		if (it->second.isMember(executer.getNickName())) {
			it->second.sendToMembers(executer, reply, cls);
		}
	}
}
