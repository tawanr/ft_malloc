LIBFT_DIR = libft

SRCS = ft_malloc.c

SRC_DIR = srcs/
OBJS = $(SRCS:.c=.o)
OBJ_DIR = objs/

CC = gcc -Ilibft
CFLAGS = -Wextra -Wall -Werror -Ilibft

NAME = libft_malloc.so
INCLUDES = -Ilibft -Iincludes

LIBS = -L$(LIBFT_DIR) -lft

$(OBJ_DIR)%o: $(SRC_DIR)%c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -fPIC $< -o $@

all: $(NAME)

$(NAME): $(addprefix $(OBJ_DIR), $(OBJS))
	$(MAKE) -C ./libft
	$(CC) $(addprefix $(OBJ_DIR), $(OBJS)) $(INCLUDES) $(LIBS) -shared -o $(NAME)

test:
	$(CC) main.c -L. -lft_malloc -o main_test

clean:
	$(MAKE) clean -C ./libft
	rm -rf $(addprefix $(OBJ_DIR), $(OBJS))
	rm -rf main_test

fclean: clean
	$(MAKE) fclean -C ./libft
	rm -rf $(NAME)

re: fclean all
