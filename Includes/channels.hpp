# ifndef CHANNELS_HPP
#define CHANNELS_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <vector>
#include <map>
#include <algorithm>
#include "clients.hpp"
#include "command.hpp"

#define NUMODES 4
#define I 0
#define T 1
#define K 2
#define L 3
#define O 4

typedef struct s_modes {
	bool active;
	int	limit;
	std::string key;
	int	flag;
}	t_modes;


class Channel
{
	private:
		std::string					_name;
		std::vector<std::string>	_members;
		std::vector<std::string>	_invites;
		std::string					_passw;
		bool						_modes[4];
		std::string					_limit;
		std::string					_topic;
	public:
		Channel();
		Channel(std::string const &name, std::string const &op);
		Channel(Channel const &ch);
		Channel &operator=(Channel const &ch);
		~Channel();
		///////////// getters /////////////
		std::string getName() const;
		std::string getPassw() const;
		std::string getTopic() const;
		int			getSizeMembers() const;
		std::string getLimit() const;
		int			getLimit_int() const;
		t_modes		*getModes() const;
		std::string getModies() const;
		std::string	getMember(std::string const &nick) const;
		std::string	getAllMembers() const;

		///////////// isChecks /////////////
		bool isTopic() const;
		bool isInviteOnly() const;
		bool isKey() const;
		bool isLimit() const;
		bool isInvited(std::string const &nick) const;
		bool isMember(std::string const &nick) const;
		bool isOperator(std::string const &nick) const;

		///////////// setters /////////////
		void setPassw(std::string const &pass);
		void setTopic(std::string const &newTopic);
		void setLimit(int limit);
		void setTopicMode(bool b);
		void setInviteOnly(bool b);
		void setKeyMode(bool b);
		void setLimitMode(bool b);
		void setOperator(std::string const &nick);

		///////////// functions /////////////
		void addMember(std::string const &nick);
		void removeMember(std::string const &nick);
		void addInvited(std::string const &nick);
		void removeInvited(std::string const &nick);
		void unsetOperator(std::string const &nick);
		void sendToMembers(Client &executer, std::string const &msg, Clients &cls);
		void sendToMembers(std::string const &msg, Clients &cls);
		void updateNick(Client &executer, Clients &cls, std::string const &old_nick, std::string const &new_nick);
};

class Channels
{
	private:
		std::map<std::string, Channel> 						_channs;
		typedef std::map<std::string, Channel>::iterator	_channsIt;
	public:
		Channels();
		Channels(Channels const &chs);
		Channels &operator=(Channels const &chs);
		~Channels();

		bool 	isChannel(std::string const &name);
		void 	addChannel(std::string const &name, std::string const &ope);
		void 	removeChann(std::string const &name);
		Channel &operator[](std::string const &ch);

		void removeClient(Client &cl, Clients &cls);
		void updateNick(Client &executer, Clients &cls, std::string const &old_nick, std::string const &new_nick);
		void	quit(Client &executer, Clients &cls);
};

#endif
