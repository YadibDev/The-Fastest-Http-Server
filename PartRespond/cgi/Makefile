CC = c++
FLAG = -Wall -Wextra -Werror -std=c++98
Name = CGI
Headers = clsParseOutCGI.hpp clsCGI.hpp Utils/HelperFunctions.hpp ../response/clsErrorPage.hpp 
SOURCE = main.cpp clsParseOutCGI.cpp clsCGI.cpp ../../Utils/HelperFunctions.cpp ../response/clsErrorPage.cpp ../../Parser/RequestHandler/RequestHandler.cpp\
		../../Utils/HttpError.cpp
OBJ = $(SOURCE:.cpp=.o)
gotoman: $(Name)

$(Name) : $(OBJ)
		$(CC) $(FLAG) $(OBJ) -o $@
%.o: %.cpp $(Headers)
	$(CC) $(FLAG) -c $<  -o $@
clean:
	rm -rf $(OBJ)
fclean: clean
	rm -rf $(Name)
.PHONY: clean
