#include "../Includes/command.hpp"

#define INVALIDNUMBER -1

int getNumber(std::string str) {
	char *end;

	long nb = std::strtol(str.c_str(), &end, 10);
	if (*end || nb <= 0 || nb >= std::numeric_limits<int>::max())
		return (INVALIDNUMBER);
	return (nb);
}

t_modes* getNewModes(Client executer, std::vector<std::string> &args, Channels &channs) {
	t_modes *newModes = channs[args[0]].getModes();
	bool s = true;
	size_t ii;
	for (size_t i = 1; i < args.size(); i++) {
		ii = i;
		for (size_t j = 0; j < args[i].size(); j++) {
			try {
				switch (args[i].at(j)) {
					case '+':
						s = true;
						break;
					case '-':
						s = false;
						break;
					case 'i':
						newModes[I].active = s;
						newModes[I].flag = 1;
						break;
					case 't':
						newModes[T].active = s;
						newModes[T].flag = 1;
						break;
					case 'l':
						newModes[L].active = s;
						if (!s)
							newModes[L].flag = -1;
						if (ii + 1 == args.size() && s)
							throw Command::Replies(ERR_NEEDMOREPARAMS(executer.getNickName(), executer.getHostId(), "MODE l"));
						else if (ii + 1 < args.size() && s) {
							newModes[L].limit = getNumber(args[++ii]);
							newModes[L].flag = 1;
						}
						break;
					case 'k':
						newModes[K].active = s;
						if (ii + 1 == args.size())
							throw Command::Replies(ERR_NEEDMOREPARAMS(executer.getNickName(), executer.getHostId(), "MODE k"));
						else if (ii + 1 < args.size()) {
							newModes[K].key = args[++ii];
							newModes[K].flag = 1;
						}
						break;
					case 'o':
						newModes[O].active = s;
						if (ii + 1 < args.size()) {
							newModes[O].key = args[++ii];
							newModes[O].flag = 1;
						}
						break;
					default:
						if (args[i][j] != 's' && args[i][j] != 'n')
							throw Command::Replies(ERR_UNKNOWNMODE(executer.getHostId(), executer.getNickName(), args[i][j]));
				}
			}
			catch (std::exception const &e) {
				std::string rep = e.what();
				send(executer.getCd(), rep.data(), rep.size(), 0);
			}
		}
		i = ii;
	}
	return (newModes);
}

void topicMode(t_modes *nModes, Channel &ch, std::string &res)
{
	if (nModes[T].flag != 1)
		return ;
	if (!nModes[T].active && ch.isTopic())
	{
		ch.setTopicMode(false);
		nModes[T].flag = -1;
		res += " -t";
	}
	else if (nModes[T].active && !ch.isTopic())
	{
		ch.setTopicMode(true);
		nModes[T].flag = 1;
		res += " +t";
	}
	else
		nModes[T].flag = 0;
}

void InviteOnlyMode(t_modes *nModes, Channel &ch, std::string &res) {
	if (nModes[I].flag != 1)
		return ;
	if (!nModes[I].active && ch.isInviteOnly()) {
		ch.setInviteOnly(false);
		nModes[I].flag = -1;
		if (nModes[T].flag == -1)
			res += "i";
		else
			res += " -i";
	}
	else if (nModes[I].active && !ch.isInviteOnly()) {
		ch.setInviteOnly(true);
		nModes[I].flag = 1;
		if (nModes[T].flag == 1)
			res += 'i';
		else
			res += " +i";
	}
	else
		nModes[I].flag = 0;
}

void keyMode(Client executer, t_modes *nModes, Channel &ch, std::string &res) {
	if (!nModes[K].flag)
		return ;
	if (nModes[K].active && ch.isKey()) {
		nModes[K].flag = 0;
		throw Command::Replies(ERR_KEYSET(executer.getHostId(), executer.getNick(), ch.getName()));
	}
	else if (!nModes[K].active && ch.isKey())
	{
		if (nModes[K].key.compare(ch.getPassw())) {
			nModes[K].flag = 0;
			throw Command::Replies(ERR_KEYSET(executer.getHostId(), executer.getNick(), ch.getName()));
		}
		ch.setKeyMode(false);
		nModes[K].flag = -1;
		if (nModes[T].flag == -1 || nModes[I].flag == -1 || nModes[L].flag == -1)
			res += 'k';
		else
			res += " -k";
	}
	else if (nModes[K].active && !ch.isKey()) {
		std::istringstream ss(nModes[K].key);
		std::string key;
		std::string tmp;
		while (ss >> tmp)
			key += tmp;
		if (key.empty()) {
			nModes[K].flag = 0;
			throw Command::Replies(ERR_INVALIDKEY(executer.getNick(), ch.getName()));
		}
		ch.setKeyMode(true);
		ch.setPassw(nModes[K].key);
		nModes[K].flag = 1;
		if (nModes[T].flag == 1 || nModes[I].flag == 1 || nModes[L].flag == 1)
			res += 'k';
		else
			res += " +k";
	}
	else
		nModes[K].flag = 0;
}

void limitMode(t_modes *nModes, Channel &ch, std::string &res) {
	if (!nModes[L].flag)
		return ;
	if (!nModes[L].active && ch.isLimit()) {
		ch.setLimitMode(false);
		nModes[L].flag = -1;
		if (nModes[T].flag == -1 || nModes[I].flag == -1)
			res += "l";
		else
			res += " -l";
	}
	else if (nModes[L].active && nModes[L].limit != INVALIDNUMBER) {
		ch.setLimitMode(true);
		ch.setLimit(nModes[L].limit);
		nModes[L].flag = 1;
		if (nModes[T].flag == 1 || nModes[I].flag == 1)
			res += "l";
		else
			res += " +l";
	}
	else
		nModes[L].flag = 0;
}

void operatorMode(Client executer, t_modes *nModes, Channel &ch, Clients &cls, std::string &res)
{
	if (!nModes[O].flag)
		return ;
	if (cls.searchNickNameClient(nModes[O].key)) {
		nModes[O].flag = 0;
		throw Command::Replies(ERR_NOSUCHNICK(executer.getHostId(), executer.getNick(), nModes[O].key));
	}
	if (!ch.isMember(nModes[O].key) && !ch.isMember("@" + nModes[O].key)) {
		nModes[O].flag = 0;
		throw Command::Replies(ERR_USERNOTINCHANNEL(executer.getHostId(), executer.getNick(), nModes[O].key, ch.getName()));
	}
	if (!nModes[O].active && ch.isOperator(nModes[O].key)) {
		ch.unsetOperator(nModes[O].key);
		nModes[O].flag = -1;
		if (nModes[T].flag == -1 || nModes[I].flag == -1 || nModes[L].flag == -1 || nModes[K].flag == -1)
			res += 'o';
		else
			res += " -o";
	}
	else if (nModes[O].active && !ch.isKey() && !ch.isOperator(nModes[O].key)) {
		ch.setOperator(nModes[O].key);
		nModes[O].flag = 1;
		if (nModes[T].flag == 1 || nModes[I].flag == 1 || nModes[L].flag == 1 || nModes[K].flag == -1)
			res += 'o';
		else
			res += " +o";
	}
	else
		nModes[O].flag = 0;
}

void Command::mode(Client &executer, std::vector<std::string> args, Channels &channs, Clients &cls)
{
	try {
		if (args.size() < 1)
			throw Command::Replies(ERR_NEEDMOREPARAMS(executer.getHostId(), executer.getNickName(), "MODE"));
		else if (!channs.isChannel(args[0]))
			throw Command::Replies(ERR_NOSUCHCHANNEL(executer.getHostId(), executer.getNick(), args[0]));
		else if (args.size() > 1 && !channs[args[0]].isMember(executer.getNickName()))
			throw Command::Replies(ERR_NOTONCHANNEL(executer.getHostId(), args[0]));
		else if (args.size() == 1 && (channs[args[0]].isMember(executer.getNickName())))
			throw Command::Replies(RPL_CHANNELMODEIS(executer.getHostId(), executer.getNick(), channs[args[0]].getName(), channs[args[0]].getModies()));
		else if (!channs[args[0]].isOperator(executer.getNick()))
			throw Command::Replies(ERR_CHANOPRIVSNEEDED(executer.getHostId(), executer.getNick(), channs[args[0]].getName()));
		t_modes *nModes = getNewModes(executer, args, channs);
		std::string res = ":" + executer.getNickName() + "!~" + executer.getNickName() + "@" + executer.getHostId() + " MODE " + args[0] + " ";
		topicMode(nModes, channs[args[0]], res);
		InviteOnlyMode(nModes, channs[args[0]], res);
		limitMode(nModes, channs[args[0]], res);
		try { keyMode(executer, nModes, channs[args[0]], res); } catch (const std::exception &e) {
			std::string rep = e.what();
			send(executer.getCd(), rep.data(), rep.size(), 0);
		}
		try { operatorMode(executer, nModes, channs[args[0]], cls, res); } catch (const std::exception &e) {
			std::string rep = e.what();
			send(executer.getCd(), rep.data(), rep.size(), 0);
		}
		nModes[L].flag == 1 ? res += " " + channs[args[0]].getLimit() : res += "";
		(nModes[K].flag == 1 || nModes[K].flag == -1) ? res += " " + nModes[K].key : res += "";
		nModes[O].flag != 0 ? res += " " + nModes[O].key : res += "";
		res += "\r\n";
		if (nModes[T].flag || nModes[I].flag || nModes[K].flag || nModes[L].flag || nModes[O].flag) {
			channs[args[0]].sendToMembers(res, cls);
		}
	}
	catch (const std::exception &e) {
		std::string rep = e.what();
		send(executer.getCd(), rep.data(), rep.size(), 0);
	}
}
