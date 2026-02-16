CPP = cc 
CPPFLAGS = -Wall -Wextra -Werror --std=c++98
NAME = webserv

# all directorys
# DIR_PARS = parsing

# # convert names from .cpp to .o
# LOCAL_FILES = main.cpp
# LOCAL_OBJECT = $(LOCAL_FILES:.c=.o)

# _PARSING = check_maze.c erno_pars.c get_map.c get_next_line.c characters_check.c\
# 		get_next_line_utils.c is_valid_file.c mini_list.c \
# 		parsing_color.c parsing_utils.c strings.c parsing_texteur.c all_direction_2.c all_directions.c

# PARSING_FILES = $(addprefix $(DIR_PARS)/, $(_PARSING))
# PARSING_OBJECT = $(PARSING_FILES:.c=.o)


# all: $(MLX_NAME) $(NAME)

# $(NAME): $(PARSING_OBJECT) $(LOCAL_OBJECT) $(RAY_OBJECT)
# 	$(CC) $(CFLAGS) $(MLX_FLAG) $^ -o $@

# $(DIR_PARS)/%.o: $(DIR_PARS)/%.c $(DIR_PARS)/parsing.h
# 	$(CC) $(CFLAGS) -c $< -o $@

# $(DIR_RAY)/%.o: $(DIR_RAY)/%.c $(DIR_RAY)/raycast.h $(DIR_PARS)/parsing.h
# 	$(CC) $(CFLAGS) -c $< -o $@

# %.o: %.c $(DIR_RAY)/raycast.h $(DIR_PARS)/parsing.h
# 	$(CC) $(CFLAGS) -c $< -o $@

# $(MLX_NAME):
# 	@make -C mlx

# clean:
# 	rm -f $(PARSING_OBJECT) $(LOCAL_OBJECT) $(RAY_OBJECT)
# 	@make -C Cub3dBonus clean

# fclean: clean
# 	rm -f $(NAME)

# re:	fclean all

# .PHONY: clean



# compiler settings
CPP = g++
# use C++11 to avoid initializer-list complaints and general modern features
CPPFLAGS = -Wall -Wextra -Werror -std=c++11

# directories containing source files; adjust/add as project grows
SRCDIRS := . Utils

# only the sources needed to build the `header` binary. this keeps `make`
# fast and avoids compiling unrelated parts of the project. the globbing
# approach from before would traverse the whole tree and pull in files that
# currently fail to build; the comment below reflects the user's request that
# "it should only run for Header".
# The binary built by default needs a `main` function; header_main.cpp
# contains a simple exercise of the parser.  Update or replace as needed.
HEADER_SRCS := Header.cpp \
	header_main.cpp \
	Utils/HelperFunctions.cpp \
	Utils/HelperString.cpp \
	Utils/HttpError.cpp
HEADER_OBJS := $(HEADER_SRCS:.cpp=.o)
TARGET := header

all: $(TARGET)

$(TARGET): $(HEADER_OBJS)
	$(CPP) $(CPPFLAGS) $^ -o $@

# if you later want to build the whole project you can add a separate target
# and re-introduce the recursive find, but the default build is now minimal.

# generic rule to produce object files from sources
%.o: %.cpp Header.hpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	# remove header-specific object files; also sweep any stray .o left behind
	# and explicitly wipe out objects from utility subdirectories so stale
	# files compiled with the old `cc` invocation are purged.
	rm -f $(HEADER_OBJS) *.o Utils/*.o

fclean: clean
	rm -f $(TARGET)

re: fclean all

# --- testing support --------------------------------------------------

# pick up any test sources placed under tests/
TEST_SRC := $(wildcard tests/*.cpp)
TEST_BIN := header_test

# build and run tests
# delegate to the Makefile living in tests/ so that additional test binaries
# or helpers can be added without touching the top-level makefile.
test:
	$(MAKE) -C tests
	./tests/$(TEST_BIN)

# convenience target to compile and then execute the header binary
run: $(TARGET)
	./$(TARGET)

# keep the helper variables in case someone wants to invoke them directly
$(TEST_BIN): $(TEST_SRC)
	$(CPP) $(CPPFLAGS) $^ -o $@

.PHONY: all clean fclean re test