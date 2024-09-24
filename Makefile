# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lpollini <lpollini@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2022/07/17 08:42:40 by lpollini          #+#    #+#              #
#    Updated: 2024/09/24 10:58:55 by lpollini         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv
	
CC			= c++
FLAGS		= -Wall -Wextra -g $(addprefix -I, $(INCLUDES)) -std=c++98
RM			= rm -rf

INCLUDES	= include/
OBJDIR 		= .objFiles

FILES		:= Parser Webserv utils Server BetterSelect BetterSocket Parsing_exceptions Responser Server_utils transfer_encoding receive_request
FILES		:= main $(addprefix src/, $(FILES))

SRC			= $(FILES:=.cpp)
OBJ			= $(addprefix $(OBJDIR)/, $(FILES:=.o))
HEADER		= $(INCLUDES)

#Colors:
GREEN		=	\e[92;5;118m
YELLOW		=	\e[93;5;226m
GRAY		=	\e[33;2;37m
RESET		=	\e[0m
CURSIVE		=	\e[33;3m

#Debug 
ifeq ($(DEBUG), 1)
   OPTS = -g
endif

.PHONY: all clean fclean re bonus norm

all: $(NAME)

$(NAME): $(OBJ) $(HEADER)
	$(CC) $(FLAGS) $(OBJ) $(OPTS) -o $(NAME)
	@printf "$(_SUCCESS) $(GREEN)- Executable ready.\n$(RESET)"

$(OBJDIR)/%.o: %.cpp $(HEADER)
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) $(OPTS) -c $< -o $@ 

$(OBJDIR):

class:
	.bash ./new_class

clean:
	@$(RM) $(OBJDIR) $(OBJ)
	@printf "$(YELLOW)    - Object files removed.$(RESET)\n"

fclean: clean
	@$(RM) $(NAME)
	@printf "$(YELLOW)    - Executable removed.$(RESET)\n"

re: fclean all