CPP = c++
CPPFLAGS = -Wall -Wextra  --std=c++98
NAME = webserv

SERVER_FILES = server/EpollHandler.cpp  server/ServerSock.cpp  server/testing.cpp main.cpp

UTILS_FILES = Utils/HelperFunctions.cpp  Utils/HelperFunctions.cpp    Utils/HelperString.cpp  Utils/HttpError.cpp  
RESPOND_FILES = PartRespond/mainprocess/clsMainProcess.cpp  PartRespond/response/clsResponse.cpp  PartRespond/response/clsErrorPage.cpp

ALL_FILES += $(SERVER_FILES) $(RESPOND_FILES) $(UTILS_FILES)

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
