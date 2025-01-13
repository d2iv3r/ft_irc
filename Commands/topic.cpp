#include "../Includes/command.hpp"

void Command::topic(Client executer, std::vector<std::string> args, Channels &channs, Clients &cls) {
	try {
		if (args.size() < 1 || !args[0].compare(":\r"))
			throw Command::Replies(ERR_NEEDMOREPARAMS(executer.getNickName(), this->hostname, "TOPIC"));
		else if (!channs.isChannel(args[0]))
			throw Command::Replies(ERR_NOSUCHCHANNEL(this->hostname, executer.getNickName(), args[0]));
		else if (channs[args[0]].getTopic().empty() && args.size() == 1)
			throw Command::Replies(RPL_NOTOPIC(this->hostname, executer.getNickName(), args[0]));
		else if (!channs[args[0]].getTopic().empty() && args.size() == 1)
			throw Command::Replies(RPL_TOPIC(executer.getHostId(), executer.getNick(), channs[args[0]].getName(), channs[args[0]].getTopic()));
		else if (channs[args[0]].isTopic() && !channs[args[0]].isOperator(executer.getNick()))
			throw Command::Replies(ERR_CHANOPRIVSNEEDED(this->hostname, executer.getNick(), channs[args[0]].getName()));
		channs[args[0]].setTopic(args.back());
		std::string reply = RPL_NEWTOPICSETTED(executer.getNick(), executer.getUserName(), executer.getHostId(), args[0], args.back());
		channs[args[0]].sendToMembers(reply, cls);
	}
	catch (const std::exception &e)
	{
		std::string rep = e.what();
		send(executer.getCd(), rep.data(), rep.size(), 0);
	}
}
