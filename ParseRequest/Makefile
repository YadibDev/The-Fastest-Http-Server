NAME        = uri_parser

CC          = c++
FLAGS       = -Wall -Wextra -Werror -std=c++98

SRC         = URI.cpp URIParser.cpp HelperFunctions.cpp clsStartLine.cpp main.cpp HttpError.cpp
OBJ         = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp URI.hpp URIParser.hpp clsStartLine.hpp HelperFunctions.hpp HttpError.hpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re