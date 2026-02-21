CPP = c++
CPPFLAGS = -Wall -Wextra -g --std=c++98
NAME = webserv

SERVER_FILES = server/clsEpollHandler.cpp  server/clsServerSock.cpp  server/testing.cpp serverMain.cpp \
				linker/clsLinker.cpp linker/clsClient.cpp

UTILS_FILES = Utils/HelperFunctions.cpp  Utils/HelperFunctions.cpp    Utils/HelperString.cpp  Utils/HttpError.cpp \
		Utils/Lexer.cpp Utils/HelperFunctions.cpp

RESPOND_FILES = PartRespond/mainprocess/clsMainProcess.cpp  PartRespond/response/clsResponse.cpp  PartRespond/response/clsErrorPage.cpp \
				PartRespond/response/RequestHandler.cpp

CONFIG_FILES = ParseConfigFile/ConfigFile/ParseConfigueFile.cpp ParseConfigFile/ServerConfig/ConfigDirectiveParser.cpp \
				ParseConfigFile/ServerConfig/ServerConfig.cpp ParseConfigFile/LocationConfig/LocationConfig.cpp

REQUEST_FILES = ParseRequest/URI.cpp ParseRequest/URIParser.cpp

ALL_FILES += $(SERVER_FILES) $(RESPOND_FILES) $(UTILS_FILES) $(CONFIG_FILES) $(REQUEST_FILES)

OBJ = $(ALL_FILES:.cpp=.o)
DEP = $(OBJ:%.o=%.d)

all : $(NAME) 

$(NAME) : $(OBJ) 
	$(CPP) $(CPPFLAGS)  $^ -o $(NAME)

%.o : %.cpp
	$(CPP) $(CPPFLAGS) $< -c -MMD  -o $@

clean:
	rm -rf $(OBJ) $(DEP)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEP)
