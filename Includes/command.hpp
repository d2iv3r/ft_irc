#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <utility>
#include <map>
#include <sys/socket.h>
#include "clients.hpp"
#include "channels.hpp"
#include "exception"
#include "replies.hpp"
#include <poll.h>
#include <unistd.h>
#include <limits>

class Channels;

class Command
{
	private:
		static std::string	_cmdsList[NUMCMDS];
		std::string sender;
		std::string receiver;
		std::string hostname;
		std::vector< std::pair< std::string, std::vector<std::string> > > _cmds;
		Client			_executer;
		int valid;
		int flag;
		int flag_help;
		int lock;
		int flag_command;
		bool _quit;
		Command()
		{
			valid = 0;
			flag = 0;
			flag_help = 0;
			lock = 0;
			flag_command = 0;
		};
		static void init();
	public:
		class Replies : public std::exception {
			private:
				std::string	_rep;
			public:
				Replies(std::string _rep);
				const char* what() const throw();
				virtual ~Replies() throw();
		};
		Command(std::string &buff, Client &executer);
		~Command();
		bool isQuited() const;

		void sendToChannel(std::string const &msg, Channels const &chan, Clients const &cls);

		std::vector<std::pair<std::string, std::vector<std::string> > > get_cmd();
		void setNameSender(std::string sender);
		void setNameReceiver(std::string receiver);
		int isCommand(std::string const &str) const;
		void execute(Client &executer, Clients &cls, Channels &channs, std::string _psswd, std::vector<pollfd> &pfds);
		std::vector<std::pair<std::string, std::vector<std::string> > > get_cmds() const;

		void password(Client &_client, std::vector<std::string> args,  std::string _psswd);
		void nickName(Client &_client, std::vector<std::string> args, Clients &cl, std::string _psswd, Channels &channs);
		void userName(Client &_client, std::vector<std::string> args, Clients &cls,  std::string _psswd);

		void mode(Client &executer, std::vector<std::string> args, Channels &channs, Clients &cls);
		void kick(Client executer, std::vector<std::string> args, Channels &channs, Clients &cls);
		void invite(Client executer, std::vector<std::string> args, Channels &channs, Clients &cls);
		void topic(Client executer, std::vector<std::string> args, Channels &channs, Clients &cls);
		void prvMsg(Client &executer, std::vector<std::string> &args, Channels &channs, Clients &cls);
		void join(Client &executer, std::vector<std::string> &args, Channels &channs, Clients &cls);
		std::pair< std::string, std::vector<std::string> > parseTheMessage(std::string buff);
		void replayJoin(Client &executer, std::string channel, Channels &channs,bool flag, Clients &cls);
		void quit(Client &executer, std::vector<std::string> args, Clients &cls, Channels &channs, std::vector<pollfd> &pfds);
};

#endif
