#include "../Includes/command.hpp"

void Command::kick(Client executer, std::vector<std::string> args, Channels &channs, Clients &cls) {
	try {
		if (args.size() < 2 || (args.size() == 2 && !args[args.size() - 1].compare(":\r")))
			throw Command::Replies(ERR_NEEDMOREPARAMS(executer.getNickName(), this->hostname, "KICK"));
		else if (!channs.isChannel(args[0]))
			throw Command::Replies(ERR_NOSUCHCHANNEL(this->hostname, executer.getNickName(), args[0]));
		else if (args.size() == 1 || !cls.isClient(args[1]))
			throw Command::Replies(ERR_NOSUCHNICK(this->hostname, executer.getNick(), args[1]));
		else if (!channs[args[0]].isOperator(executer.getNick()))
			throw Command::Replies(ERR_CHANOPRIVSNEEDED(this->hostname, executer.getNickName(), args[0]));
		else if (!channs[args[0]].isMember(args[1]))
			throw Command::Replies(ERR_USERNOTINCHANNEL(this->hostname, executer.getNickName(), args[1], args[0]));
		std::string reason = "";
		if (args.size() > 2 && args[2].compare(":\r"))
			reason = " :" + args.back();
		std::string reply = RPL_KICK(executer.getNickName(), executer.getUserName(), executer.getHostId(), args[1], args[0], reason);
		channs[args[0]].sendToMembers(reply, cls);
		channs[args[0]].removeMember(args[1]);
		if (channs[args[0]].isInvited(args[1]))
			channs[args[0]].removeInvited(args[1]);
		if (channs[args[0]].getSizeMembers() == 0)
			channs.removeChann(args[0]);
	}
	catch(const std::exception &e)
	{
		std::string rep = e.what();
		send(executer.getCd(), rep.data(), rep.size(), 0);
	}
}
