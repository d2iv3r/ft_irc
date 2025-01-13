#include "../Includes/command.hpp"

void Command::quit(Client &executer, std::vector<std::string> args, Clients &cls, Channels &channs, std::vector<pollfd> &pfds)
{
	(void)pfds;
	std::string message;
	if (args.size() == 0)
	{
		message = "Leaving...";
	}
	else {
		message = args[0];
	}
	std::string buff = RPL_QUIT(executer.getNickName(), executer.getHostId(), message);
	std::cout << buff << std::endl;
	send(executer.getCd(), buff.data(), buff.size(), 0);
	channs.quit(executer, cls);
	channs.removeClient(executer, cls);
	cls.eraseClient(executer.getCd());
	close(executer.getCd());
	this->_quit = true;
}

