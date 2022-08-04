NAME = webserv

SRCS = main.cpp

HEADERS	= inc/Webserv.hpp

OBJS = $(SRCS:.cpp=.o)

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

RM = rm -rf

RESET = "\033[0m"
BLACK = "\033[1m\033[37m"

all: $(NAME)

$(NAME) : $(OBJS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo $(BLACK)-webserv compiled 🌐 $(RESET)

clean:
		$(RM) $(NAME)

fclean: clean
		$(RM) $(OBJS)
		@echo OBJ files removed

re: 	fclean all

.PHONY: all clean fclean re