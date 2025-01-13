#ifndef BOT_HPP
#define BOT_HPP

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <netdb.h>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <fstream>

#define REGISTER(nick, pass) "PASS " + pass + "\nNICK " + nick + "\nUSER kicki kicki bot bot\n"
#define RPLY_HELLO(nick) "Hello " + nick + ", My name is KICKI, I'am a irc bot, my job is kick members whos use rude words in your channels! just invite me in the channel as operator and i will do the job for you\n"
#define RPLY_HELP "To enable KICKI : INVITE KICKI <#your-channel> + MODE <#your-channel> +o KICKI\nTo disable KICKI : KICK KICKI <#your-channel>\n"
#define RPLY_INVITATION(nick, channel) "I receive your invitation mr." + nick + " to channel " + channel + ". just give me the operator prevelige and don't worry for your channel\n"
#define RPLY_CHANNEL(channel) "Hello " + channel + " members, I'd like to informe you about a new rule setted in the channel, that any one use rude language will kicked out from the channel\n"


class Bot
{
	private:
		int							_sock;
		std::string					_nickName;
		std::string					_host;
		int							_port;
		std::string					_pass;
		std::string					_nick;
		std::string					_message;
		std::string					_channel;
		std::vector<std::string>	_data;
	public:
		Bot();
		Bot(std::string const &host, std::string const &port, std::string const &pass, std::string const &nick);
		void start();
		void regist();
		void listenMsg();
		void parseReq(std::string buff);

		bool isClientPrivmsg(std::string buff);
		bool isInvite(std::string buff);
		bool isChannelPrivmsg(std::string buff);
		bool isSwear(std::string message);
};

#endif
