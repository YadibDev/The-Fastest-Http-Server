

CC = c++

FLAG = -Wall -Wextra -Werror -g -std=c++98
Name = CGI
HEADERS = clsParseOutCGI.hpp clsCGI.hpp Utils/HelperFunctions.hpp ../response/clsErrorPage.hpp
SOURCE = main.cpp clsParseOutCGI.cpp clsCGI.cpp ../../Utils/HelperFunctions.cpp ../response/clsErrorPage.cpp

OBJ = $(SOURCE:.cpp=.o)
gotoman: $(Name)

$(Name) : $(OBJ)
	$(CC) $(FLAG) $(OBJ) -o $@
 %.o: %.cpp #$(HEADERS)
	$(CC) $(FLAG) -c $< -o $@
clean:
	rm -rf $(OBJ)
fclean: clean
	rm -rf $(Name)
re : fclean gotoman
.PHONY: clean