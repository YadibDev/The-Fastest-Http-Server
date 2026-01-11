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