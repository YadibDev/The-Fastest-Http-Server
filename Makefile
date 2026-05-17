CPP = c++
CPPFLAGS = -Wall -Wextra -Werror -g --std=c++98 #-fsanitize=address
NAME = webserv

SERVER_FILES = server/clsEpollHandler.cpp  server/clsServerSock.cpp  server/testing.cpp serverMain.cpp \
				 linker/clsClient.cpp linker/clsFlow.cpp

UTILS_FILES = Utils/HelperFunctions.cpp  Utils/HelperFunctions.cpp  Utils/HttpError.cpp \
		Utils/Lexer.cpp


RESPOND_FILES =	 PartRespond/mainprocess/clsMainProcess.cpp  PartRespond/response/clsResponse.cpp PartRespond/response/clsErrorPage.cpp PartRespond/response/clsResponse.cpp \
					PartRespond/cgi/clsCGI.cpp PartRespond/cgi/clsParseOutCGI.cpp PartRespond/response/clsAutoIndex.cpp 


CONFIG_FILES = Parser/ParseConfigFile/ConfigFile/ParseConfigueFile.cpp Parser/ParseConfigFile/LocationConfig/LocationConfig.cpp \
				Parser/ParseConfigFile/ServerConfig/ConfigDirectiveParser.cpp Parser/ParseConfigFile/ServerConfig/ServerConfig.cpp

REQUEST_FILES = Parser/ParseRequest/Request/Header.cpp Parser/ParseRequest/Request/HeaderTable.cpp Parser/ParseRequest/Request/RequestLine.cpp Parser/ParseRequest/Request/RequestParser.cpp \
				Parser/ParseRequest/Request/Utils.cpp Parser/ParseRequest/URI/NUriParser.cpp Parser/ParseRequest/URI/URI.cpp Parser/ParseRequest/URI/URIParser.cpp \
				Parser/RequestHandler/ProcessRequestHandler.cpp Parser/RequestHandler/RequestHandler.cpp Parser/RequestHandler/PathResolver.cpp Parser/ParseRequest/Request/clsBody.cpp \

ALL_FILES +=  $(UTILS_FILES) $(CONFIG_FILES) $(REQUEST_FILES) $(RESPOND_FILES) $(SERVER_FILES)

OBJ = $(ALL_FILES:.cpp=.o)
DEP = $(OBJ:%.o=%.d)

all : $(NAME)

$(NAME) : $(OBJ)
	$(CPP) $(CPPFLAGS)  $^ -o $(NAME)

%.o : %.cpp $(Header)
	$(CPP) $(CPPFLAGS) $< -c -MMD  -o $@

clean:
	rm -rf $(OBJ) $(DEP)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEP)
