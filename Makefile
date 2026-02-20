CPP = c++
CPPFLAGS = -Wall -Wextra -g --std=c++98
NAME = webserv

SERVER_FILES = server/EpollHandler.cpp  server/ServerSock.cpp  server/testing.cpp main.cpp

UTILS_FILES = Utils/HelperFunctions.cpp  Utils/HelperFunctions.cpp    Utils/HelperString.cpp  Utils/HttpError.cpp \
		Utils/Lexer.cpp

RESPOND_FILES = Respond/Respond.cpp Respond/RespondAutoIndex.cpp Respond/RespondCGI.cpp Respond/RespondErrorPage.cpp \
				Respond/RespondFile.cpp Respond/RespondRedirection.cpp


CONFIG_FILES = Parser/ParseConfigFile/ConfigFile/ParseConfigueFile.cpp Parser/ParseConfigFile/LocationConfig/LocationConfig.cpp \
				Parser/ParseConfigFile/ServerConfig/ConfigDirectiveParser.cpp Parser/ParseConfigFile/ServerConfig/ServerConfig.cpp

REQUEST_FILES = Parser/ParseRequest/Request/clsStartLine.cpp Parser/ParseRequest/Request/Header.cpp Parser/ParseRequest/Request/Request.cpp Parser/ParseRequest/URI/URI.cpp \
				Parser/ParseRequest/URI/URIParser.cpp Parser/RequestHandler/ProcessRequestHandler.cpp Parser/RequestHandler/RequestHandler.cpp

ALL_FILES += $(SERVER_FILES) $(UTILS_FILES) $(CONFIG_FILES) $(REQUEST_FILES)

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
