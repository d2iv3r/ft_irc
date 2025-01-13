NAME = ircserv

NAME_BONUS = kicki_bot

CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

INCLUDES = Includes/server.hpp Includes/replies.hpp Includes/command.hpp Includes/clients.hpp Includes/channels.hpp

COMMANDS = Commands/command.cpp Commands/replies.cpp Commands/authentication.cpp Commands/mode.cpp Commands/invite.cpp\
			Commands/kick.cpp Commands/topic.cpp Commands/join.cpp Commands/prvMsg.cpp Commands/quit.cpp\

CLIENTS = Clients/clients.cpp

BONUS = Bot/bot.cpp

BONUS_OBJ = $(BONUS:.cpp=.o)

CHANNELS = Channels/channel.cpp Channels/channels.cpp

SRC = main.cpp server.cpp $(COMMANDS) $(CLIENTS) $(CHANNELS)

OBJ = $(SRC:.cpp=.o)

all : $(NAME)

bonus : $(NAME_BONUS)

$(NAME) : $(OBJ) $(INCLUDES)
	$(CC) $(FLAGS) $(OBJ) -o $@

$(NAME_BONUS) : $(BONUS_OBJ) Includes/bot.hpp
	$(CC) $(FLAGS) $(BONUS_OBJ) -o $@

%.o : %.cpp $(INCLUDES)
	$(CC) $(FLAGS) -c $< -o $@

clean :
	rm -rf $(OBJ)
	rm -rf $(BONUS_OBJ)

fclean : clean
	rm -rf $(NAME) $(NAME_BONUS)

re : fclean all bonus

.phony : all clean fclean re
