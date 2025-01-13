/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hachahbo <hachahbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/16 01:04:22 by hachahbo          #+#    #+#             */
/*   Updated: 2024/04/22 19:56:01 by hachahbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/command.hpp"

int parsetheSrt(std::string str, bool b)
{
    if (b == 1)
    {
        if (str.find_first_not_of("#abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789") != std::string::npos)
            return (0);

    }
    return (1);
}
std::vector<std::string> splitString(std::string &s, char delimiter, bool b, Client &executer, std::string hostname)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        if (token.size() > 0)
        {
            if (!s.empty() && token[0] != '#' && b == 1)
            {
                token = '#' + token;
            }
            if((token[0] == '#' && token.size() == 1) || !parsetheSrt(token, b))
            {
                std::string buff = ERR_BADCHANNELNAME(executer.getNickName(), hostname, token);
                send(executer.getCd(), buff.c_str(), buff.size(), 0);
                std::cout << buff << std::endl;
                continue;
            }
            tokens.push_back(token);
        }
    }
    return tokens;
}

std::vector<std::string> splitString(std::string &s, char delimiter, bool b)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        if (!s.empty() && token[0] != '#' && b == 1)
        {
            token = '#' + token;
        }
        if (parsetheSrt(token, b))
            tokens.push_back(token);
    }
    return tokens;
}

void Command::replayJoin(Client &executer, std::string channel, Channels &channs, bool flag, Clients &cls)
{
    std::string member;
    try{
        if (flag == 1)
            member = channs[channel].getMember('@' + executer.getNickName());
        else
            member = channs[channel].getMember(executer.getNickName());
        std::string buff = RPL_JOIN(executer.getNickName(), executer.getUserName(), channel, executer.getHostId());
        send(executer.getCd(), buff.c_str(), buff.size(), 0);
        if (flag_command >= 1)
        {
            channs[channel].sendToMembers(executer, buff, cls);
        }
        std::cout << buff << std::endl;
        std::string var = channs[channel].getAllMembers();
        buff = RPL_NAMREPLY(this->hostname, channs[channel].getAllMembers(), channel, member);
        send(executer.getCd(), buff.c_str(), buff.size(), 0);
        std::cout << buff << std::endl;
        buff = RPL_ENDOFNAMES(this->hostname, executer.getNick(), channel);
        send(executer.getCd(), buff.c_str(), buff.size(), 0);
        std::cout << buff << std::endl;
        flag_command++;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void Command::join(Client &executer, std::vector<std::string> &args, Channels &channs , Clients &cls)
{
    size_t i = 0;
    if (args.size() == 0 || (!args.at(0).find("#") && args.at(0).size() == 1))
    {
        std::string buff = ERR_NEEDMOREPARAMS(executer.getNickName(), this->hostname, "JOIN");
        send(executer.getCd(), buff.c_str(), buff.size(), 0);
        std::cout << buff << std::endl;
        return;
    }
    std::vector<std::string> channels = splitString(args.at(0), ',', 1, executer, this->hostname);
    std::vector<std::string> keys;
    int flag = 1;
    if (args.size() == 1)
        flag = 0;
    // else if (args.size() > 2)
    // {
    //     std::string buff = ERR_NEEDMOREPARAMS(executer.getNickName(), this->hostname, "JOIN");
    //     send(executer.getCd(), buff.c_str(), buff.size(), 0);
    //     std::cout << buff << std::endl;
    //     return;
    // }
    else
    {
        keys = splitString(args.at(1), ',', 0);
    }

    if (args.empty())
        std::cout << "the args is empty" << std::endl;
    if (flag == 1 && channels.size() > keys.size())
    {
        size_t j = 0;
        while (j < channels.size() - keys.size())
        {
            keys.push_back("");
        }
    }
    else if (flag == 1 && channels.size() < keys.size())
    {
        size_t j = 0;
        while (j < keys.size() - channels.size())
        {
            channels.push_back("");
            j++;
        }
    }
    i = 0;
    while (i < channels.size())
    {
        std::string channel = channels.at(i);
        std::string key;

        if (args.size() >= 2)
        {
            if (keys.at(i).empty())
                flag = 0;
            else
            {
                flag = 1;
                key = keys.at(i);
            }
        }
        if (channel.empty())
            return;
        if (channs.isChannel(channel))
        {
            std::string buff = RPL_JOIN(executer.getNick(), executer.getUserName(), channs[channel].getName(), executer.getHostId());

            if (channs[channel].isMember(executer.getNickName()) || channs[channel].isMember("@" + executer.getNickName()))
            {
                std::string reply = ERR_USERONCHANNEL(this->hostname, executer.getNickName(), executer.getNickName(), channel);
                send(executer.getCd(), reply.c_str(), reply.size(), 0);
                std::cout << reply << std::endl;
                return ;
            }
            else if (channs[channel].isInviteOnly() && !channs[channel].isInvited(executer.getNickName()))
            {
                std::string reply = ERR_INVITEONLY(this->hostname, executer.getNickName(), channel);
                send(executer.getCd(), reply.c_str(), reply.size(), 0);
                std::cout << reply << std::endl;
                return ;
            }
            else if (channs[channel].isLimit() && channs[channel].getLimit_int() < channs[channel].getSizeMembers() + 1)
            {

                std::string reply = ERR_CHANNELISFULL(this->hostname, executer.getNickName(), channel);
                send(executer.getCd(), reply.c_str(), reply.size(), 0);
                std::cout << reply << std::endl;
                return ;
            }
            else if (channs[channel].isInviteOnly() && channs[channel].isInvited(executer.getNickName()))
            {
                channs[channel].addMember(executer.getNickName());
                replayJoin(executer, channel, channs, 0, cls);
            }
            else if (!channs[channel].isKey())
            {
                channs[channel].addMember(executer.getNickName());
                replayJoin(executer, channel, channs, 0, cls);
            }
            else if (channs[channel].isLimit() && channs[channel].getLimit_int() >= channs[channel].getSizeMembers() + 1)
            {
                channs[channel].addMember(executer.getNickName());
                replayJoin(executer, channel, channs, 0, cls);
            }
            else
            {
                if (flag == 0 && channs[channel].isKey())
                {
                    buff = ERR_BADCHANNELKEY(executer.getNick(), this->hostname, channel);
                    send(executer.getCd(), buff.c_str(), buff.size(), 0);
                    std::cout << buff << std::endl;
                }
                else if (!channs[channel].getPassw().compare(key))
                {
                    channs[channel].addMember(executer.getNickName());
                    replayJoin(executer, channel, channs, 0, cls);
                }
                else
                {
                    buff = ERR_BADCHANNELKEY(executer.getNick(), this->hostname, channel);
                    send(executer.getCd(), buff.c_str(), buff.size(), 0);
                    std::cout << buff << std::endl;
                }
            }
        }
        else
        {
            channs.addChannel(channel, executer.getNickName());
            if (flag == 0)
                channs[channel].setKeyMode(false);
            else
            {
                channs[channel].setKeyMode(true);
                channs[channel].setPassw(key);
            }
            // channs[channel].addMember(executer.getNickName());
            channs[channel].setOperator(executer.getNickName());
            channs[channel].setTopicMode(true);
            replayJoin(executer, channel, channs, 1, cls);
            // std::cout << executer.getHostId() << "\n";
            // std::cout <<  " operator  nickname : " << channs[channel].getMember('@'+ executer.getNickName()).getNickName() << std::endl;
            // std::cout <<  " channel name : " << channs[channel].getName() << std::endl;
            // std::cout <<  " channel password : " << channs[channel].getPassw() << std::endl;
        }
        i++;
    }
    std::cout << "\n";
}
