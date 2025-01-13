#include "../Includes/channels.hpp"

Channel::Channel()
{
}

Channel::Channel(std::string const &name, std::string const &ope) : _name(name)
{
	_members.push_back("@" + ope);
	for (size_t i = 0; i < NUMODES; i++)
	{
		_modes[i] = false;
	}
	_modes[T] = true;
}

Channel::Channel(Channel const &ch)
{
	*this = ch;
}

Channel::~Channel()
{
}

////////////////////// OPERATORS //////////////////////
Channel &Channel::operator=(Channel const &ch)
{
	if (this != &ch)
	{
		_name = ch._name;
		_members = ch._members;
		_invites = ch._invites;
		_passw = ch._passw;
		for (size_t i = 0; i < NUMODES; i++)
			_modes[i] = ch._modes[i];
		_limit = ch._limit;
		_topic = ch._topic;
	}
	return (*this);
}

//////////////////////	GETTERS //////////////////////
std::string Channel::getName() const {
	return (_name);
}
std::string Channel::getPassw() const {
	return (_passw);
}
std::string Channel::getTopic() const {
	return (_topic);
}
int Channel::getSizeMembers() const {
	return (_members.size());
}
std::string Channel::getLimit() const {
	return (_limit);
}
int Channel::getLimit_int() const {
	return (std::atoi(_limit.c_str()));
}

t_modes *Channel::getModes() const
{
	static t_modes m[5];
	std::memset(m, 0, sizeof(m));
	m[I].active = isInviteOnly();
	m[T].active = isTopic();
	m[K].active = isKey();
	m[L].active = isLimit();
	return (m);
}

std::string Channel::getModies() const
{
	std::string t = "+";
	isTopic() ? t += 't' : t += "";
	isInviteOnly() ? t += 'i' : t += "";
	isLimit() ? t += 'l' : t += "";
	isKey() ? t += 'k' : t += "";
	isLimit() ? t += ' ' + _limit : t += "";
	isKey() ? t += ' ' + _passw : t += "";
	return (t);
}

std::string Channel::getMember(std::string const &nick) const {
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] == nick || _members[i] == "@" + nick)
			return (_members[i]);
	}
	return ("hamza");
}

std::string Channel::getAllMembers() const
{
	std::string buff;
	for (size_t i = 0; i < _members.size(); i++)
	{
		std::string key = _members[i];
		buff += key + " ";
	}
	return (buff);
}

//////////////////////	IS_checks //////////////////////
bool Channel::isTopic() const {
	return (_modes[T]);
}
bool Channel::isInviteOnly() const {
	return (_modes[I]);
}
bool Channel::isKey() const {
	return (_modes[K]);
}
bool Channel::isLimit() const {
	return (_modes[L]);
}

bool Channel::isInvited(std::string const &nick) const
{
	for (size_t i = 0; i < _invites.size(); i++)
	{
		if (nick == _invites[i])
			return (true);
	}
	return (false);
}

bool Channel::isMember(std::string const &nick) const
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] == nick || _members[i] == "@" + nick)
			return (true);
	}
	return (false);
}

bool Channel::isOperator(std::string const &nick) const
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (_members[i] == "@" + nick)
			return (true);
	}
	return (false);
}

//////////////////////	SETTERS //////////////////////
void Channel::setPassw(std::string const &pass) {
	_passw = pass;
}
void Channel::setTopic(std::string const &newTopic) {
	_topic = newTopic;
}
void Channel::setTopicMode(bool b) {
	_modes[T] = b;
}
void Channel::setInviteOnly(bool b) {
	_modes[I] = b;
}
void Channel::setKeyMode(bool b) {
	_modes[K] = b;
}
void Channel::setLimitMode(bool b) {
	_modes[L] = b;
}
void Channel::setLimit(int limit) {
	std::stringstream t;
	t << limit;
	_limit = t.str();
}

void Channel::setOperator(std::string const &nick)
{
	for (size_t i = 0; i < _members.size(); i++)
	{
		if (nick == _members[i]) {
			_members[i].insert(0, "@");
		}
	}
}

//////////////////////	INVITES FUNCTIONS //////////////////////
void Channel::addInvited(std::string const &nick) {
	_invites.push_back(nick);
}

void Channel::removeInvited(std::string const &nick) {
	std::vector<std::string>::iterator it;

	it = std::find(_invites.begin(), _invites.end(), nick);
	if (it != _invites.end())
		_invites.erase(it);
}

//////////////////////	MEMBERS FUNCTIONS //////////////////////
void Channel::addMember(std::string const &nick) {
	_members.push_back(nick);
}

void Channel::removeMember(std::string const &nick)
{
	std::vector<std::string>::iterator it;

	if (this->isOperator(nick))
		it = std::find(_members.begin(), _members.end(), "@" + nick);
	else if (this->isMember(nick))
		it = std::find(_members.begin(), _members.end(), nick);
	else
		return ;
	_members.erase(it);
}

void Channel::unsetOperator(std::string const &nick)
{
	std::vector<std::string>::iterator it;

	it = std::find(_members.begin(), _members.end(), "@" + nick);
	if (it != _members.end()) {
		*it = nick;
	}
}

void Channel::sendToMembers(Client &executer, std::string const &msg, Clients &cls)
{
	std::string	nick;
	int			fd;

	for (size_t i = 0; i < _members.size(); i++)
	{
		nick = _members[i];
		if(nick[0] == '@')
			nick = nick.substr(1);
		if(nick == executer.getNickName())
			continue;
		fd = cls.getClient(nick).getCd();
		send(fd, msg.data(), msg.size(), 0);
	}
}
void Channel::sendToMembers(std::string const &msg, Clients &cls)
{
	std::string	nick;
	int			fd;
	for (size_t i = 0; i < _members.size(); i++)
	{
		nick = _members[i];
		if(nick[0] == '@')
			nick = nick.substr(1);
		fd = cls.getClient(nick).getCd();
		send(fd, msg.data(), msg.size(), 0);
	}
}

void Channel::updateNick(Client &executer, Clients &cls, std::string const &old_nick, std::string const &new_nick)
{
	std::vector<std::string>::iterator it;

	it = std::find(_invites.begin(), _invites.end(), old_nick);
	if (it != _invites.end())
		*it = new_nick;
	if (this->isOperator(old_nick)) {
		it = std::find(_members.begin(), _members.end(), "@" + old_nick);
		if (it != _members.end())
			*it = "@" + new_nick;
	}
	else if (this->isMember(old_nick)) {
		it = std::find(_members.begin(), _members.end(), old_nick);
		if (it != _members.end())
			*it = new_nick;
	}
	std::string msg = NICKNAME_RPLY(old_nick, executer.getUserName(), executer.getHostId(), new_nick);
	this->sendToMembers(executer, msg, cls);
}
