CPP = c++
CPPFLAGS = -Wall -Wextra -Werror --std=c++98
NAME = webserv

SERVER_FILES = server/EpollHandler.cpp  server/ServerSock.cpp  server/testing.cpp main.cpp

ALL_FILES += $(SERVER_FILES)

OBJ = $(ALL_FILES:.cpp=.o)
DEP = $(OBJ:%.o=%.d)

all : $(NAME) 

$(NAME) : $(OBJ) 
	$(CPP) $(CPPFLAGS)  $^ -o $(NAME)

%.o : %.cpp
	$(CPP) $(CPPFLAGS) $< -c -MMD -MP  -o $@

clean:
	rm -rf $(OBJ) $(DEP)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEP)