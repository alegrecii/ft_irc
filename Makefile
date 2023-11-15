# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dcolucci <dcolucci@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/07/06 14:39:03 by mdi-paol          #+#    #+#              #
#    Updated: 2023/11/15 14:37:56 by dcolucci         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv
SRCS = main.cpp Server.cpp utils.cpp Client.cpp Command.cpp Channel.cpp
HEADER = Server.hpp utils.hpp Client.hpp
FLAGS = -Wall -Werror -Wextra -std=c++98 -g
OBJS = $(SRCS:.cpp=.o)
CC = c++

all : $(NAME)

$(NAME): $(OBJS)
	@ $(CC) $(FLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)✅ Compiled ✅$(WHITE)"

%.o: %.cpp $(HEADER)
	@ $(CC) $(FLAGS) -c $< -o $@

clean:
	@rm -f $(OBJS)
	@echo "$(BLUE)☑️ Clean OBJS ☑️$(WHITE)"

fclean: clean
	@rm -f $(NAME)
	@echo "$(BLUE)☑️ Clean OBJS & EXEC ☑️$(WHITE)"

re: fclean all

.PHONY: all clean fclean re

#      -----COLORS------

GREEN = \033[1;32m
BLUE = \033[1;34m
WHITE = \033[0m
