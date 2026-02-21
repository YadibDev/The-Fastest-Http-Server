NAME        = uri_parser

CC          = c++
FLAGS       = -Wall -Wextra -Werror -g -std=c++98

SRC         = URI.cpp URIParser.cpp ../Utils/HelperFunctions.cpp clsStartLine.cpp main.cpp Request.cpp Header.cpp ../Utils/HttpError.cpp
OBJ         = $(SRC:.cpp=.o)

TEST_SRC    = test_request.cpp Request.cpp Header.cpp
TEST_OBJ    = $(TEST_SRC:.cpp=.o)

# Objects without the program's main (used for test linking to avoid duplicate main.o)
OBJ_NOMAIN  = $(filter-out main.o, $(OBJ))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $(OBJ) -o $(NAME)

%.o: %.cpp
	$(CC) $(FLAGS) -c $< -o $@

# legacy specific rule (kept for compatibility)
%.o: %.cpp URI.hpp URIParser.hpp clsStartLine.hpp ../Utils/HelperFunctions.hpp ../Utils/HttpError.hpp
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ) $(TEST_OBJ)

fclean: clean
	rm -rf $(NAME) test_request

re: fclean all

test: $(TEST_OBJ) $(OBJ_NOMAIN)
	$(CC) $(FLAGS) $(TEST_OBJ) $(filter-out $(TEST_OBJ), $(OBJ_NOMAIN)) -o test_request

.PHONY: all clean fclean re test