CC = c++
FLAG = -Wall -Wextra -Werror -std=c++98
Name = Rrespond
SOURCE = main.cpp  clsErrorPage.cpp clsResponse.cpp ../../Utils/HelperFunctions.cpp ../../Parser/RequestHandler/RequestHandler.cpp ../../Utils/HttpError.cpp \
		../../Parser/ParseRequest/Request/HeaderTable.cpp

OBJ = $(SOURCE:.cpp=.o)
nono: $(Name)

$(Name) : $(OBJ)
		$(CC) $(FLAG) $(OBJ) -o $@
%.o: %.cpp

	$(CC) $(FLAG) -c $<  -o $@
clean:
	rm -rf $(OBJ)
fclean: clean
	rm -rf $(Name)
.PHONY: clean
