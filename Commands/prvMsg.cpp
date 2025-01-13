/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prvMsg.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hachahbo <hachahbo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/18 02:17:20 by hachahbo          #+#    #+#             */
/*   Updated: 2024/04/20 19:57:21 by hachahbo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/command.hpp"

std::pair<std::string, std::vector<std::string> > Command::parseTheMessage(std::string buff)
{
    std::stringstream spl(buff);
    std::string word;
    std::pair<std::string, std::vector<std::string> > p;

    init();
    while (spl >> word)
    {
        if (!isCommand(word))
        {
            std::string tmp = word;
            spl >> word;
            if (isCommand(word))
                p.second.push_back(tmp);
        }
        if (isCommand(word))
        {
            p.first = word;
            while (spl >> word && !isCommand(word))
            {
                if (word.find(':', 0) != std::string::npos)
                {
                    std::string tmp;
                    std::getline(spl, tmp);
                    p.second.push_back(word + tmp);
                    break;
                }
                p.second.push_back(word);
            }
        }
    }
    return (p);
}

void Command::prvMsg(Client &executer, std::vector<std::string> &args, Channels &channs, Clients &cls)
{
    try
    {
        (void)executer;
        (void)args;
        (void)channs;
        (void)cls;

        if (args.size() <= 1)
        {
            std::string buff = ERR_NOTEXTTOSEND(executer.getNickName(), executer.getHostId());
            send(executer.getCd(), buff.c_str(), buff.size(), 0);
            std::cout << buff << std::endl;
            return;
        }
        this->receiver = args.at(0);
        std::string message = args.at(1);
        if (receiver.at(0) == '#')
        {
            if (!channs.isChannel(receiver))
            {
                std::string buff = ERR_NOSUCHCHANNEL(executer.getNickName(), executer.getHostId(), receiver);
                if (send(executer.getCd(), buff.c_str(), buff.size(), 0) == -1)
                    std::perror("send");
                return;
            }
            if (!channs[receiver].isMember(executer.getNickName()) && !channs[receiver].isMember("@" + executer.getNickName()))
            {
                std::string buff = ERR_CANNOTSENDTOCHAN(this->hostname, executer.getNickName(), receiver);
                if (send(executer.getCd(), buff.c_str(), buff.size(), 0) == -1)
                    std::perror("send");
                return;
            }
            message = PRIVMSG_FORMAT(executer.getNickName(), executer.getUserName(), executer.getHostId(), channs[args.at(0)].getName(), message);
            if (channs.isChannel(receiver))
                channs[receiver].sendToMembers(executer, message, cls);
        }
        else
        {
            Client &cl_recev = cls.findTheReciever(executer.getCd(), executer.getNickName(), this->receiver, executer.getHostId());
            int fd_recev = cl_recev.getCd();
            if (message[0] == ':')
                message.erase(0, 1);
            // #define PRIVMSG_FORMAT(senderNick, senderUsername, senderHostname, receiver, message) ":" + senderNick + "!~" + senderUsername + "@" + senderHostname + " PRIVMSG " + receiver + " :" + message + "\r\n"
            std::string buff = PRIVMSG_FORMAT(executer.getNickName(), executer.getUserName(), executer.getHostId(), cl_recev.getNickName(), message);

            if (send(fd_recev, buff.c_str(), buff.size(), 0) == -1)
                std::perror("send");
            std::cout << buff << std::endl;
        }
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
    }
}
