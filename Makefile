CPP = c++
CPPFLAGS = -Wall -Wextra -Werror --std=c++98
NAME = webserv

SERVER_FILES = server/EpollHandler.cpp  server/ServerSock.cpp  server/testing.cpp

ALL_FILES += $(SERVER_FILES)

OBJ = $(ALL_FILES:%.cpp=%.o)

all: $(NAME)

$(NAME): $(OBJ) main.cpp
	$(CPP) $(CPPFLAGS) $^ -o $@

%.o : %.cpp
	$(CPP) $(CPPFLAGS) $< -c -MMD -o $@