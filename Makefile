NAME = webserv

SRCS = main.cpp src/ConfigFile.cpp src/ConfigParser.cpp src/ServerConfig.cpp src/Location.cpp

HEADERS	= inc/Webserv.hpp

OBJS = $(SRCS:.cpp=.o)

CXX = c++

CXXFLAGS = -Wall -Wextra -Werror -std=c++98

RM = rm -rf

RESET = "\033[0m"
BLACK = "\033[1m\033[37m"

all: $(NAME)

$(NAME) : $(OBJS) 
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo $(BLACK)-webserv compiled 🌐 $(RESET)

clean:
		$(RM) $(NAME)

fclean: clean
		$(RM) $(OBJS)
		@echo OBJ files removed

re: 	fclean all

fmt:
	clang-format -i $(SRCS) $(HEADERS)

.PHONY: all clean fclean re fmt