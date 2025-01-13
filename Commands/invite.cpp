#include "../Includes/command.hpp"

void Command::invite(Client executer, std::vector<std::string> args, Channels &channs, Clients &cls) {
	try
	{
		if (args.size() < 2)
			throw Command::Replies(ERR_NEEDMOREPARAMS(executer.getNickName(), this->hostname, "INVITE"));
		if (!cls.isClient(args[0]))
			throw Command::Replies(ERR_NOSUCHNICK(this->hostname, executer.getNickName(), args[0]));
		if (!channs.isChannel(args[1]))
			throw Command::Replies(ERR_NOSUCHCHANNEL(this->hostname, executer.getNickName(), args[1]));
		if (!channs[args[1]].isMember(executer.getNickName()))
			throw Command::Replies(ERR_NOTONCHANNEL(this->hostname, args[1]));
		if (channs[args[1]].isMember(args[0]))
			throw Command::Replies(ERR_USERONCHANNEL(this->hostname, executer.getNickName(), args[0], args[1]));
		if (!channs[args[1]].isOperator(executer.getNickName()))
			throw Command::Replies(ERR_CHANOPRIVSNEEDED(this->hostname, executer.getNickName(), args[1]));
		channs[args[1]].addInvited(args[0]);
		std::string reply = RPL_INVITE(executer.getNickName(), executer.getUserName(), executer.getHostId(), args[1]);
		send(cls.getClient(args[0]).getCd(), reply.data(), reply.size(), 0);
		throw Command::Replies(RPL_INVITING(executer.getHostId(), executer.getNickName(), args[0], args[1]));
	}
	catch (const std::exception &e)
	{
		std::string rep = e.what();
		send(executer.getCd(), rep.data(), rep.size(), 0);
	}
}
