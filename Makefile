LIBFT_DIR = libft

ifeq ($(HOSTTYPE),)
HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

SRCS = mem_blocks.c show_alloc_mem.c ft_malloc.c ft_free.c ft_realloc.c

SRC_DIR = srcs/
OBJS = $(SRCS:.c=.o)
OBJ_DIR = objs/

CC = gcc -Ilibft
CFLAGS = -Wextra -Wall -Werror -Ilibft

NAME = libft_malloc_$(HOSTTYPE).so
SYMLINK = libft_malloc.so
INCLUDES = -Ilibft -Iincludes

LIBS = -L$(LIBFT_DIR) -lft

$(OBJ_DIR)%o: $(SRC_DIR)%c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -c -fPIC $< -o $@

all: $(NAME)

$(NAME): $(addprefix $(OBJ_DIR), $(OBJS))
	$(MAKE) -C ./libft
	$(CC) $(addprefix $(OBJ_DIR), $(OBJS)) $(INCLUDES) $(LIBS) -shared -o $(NAME)
	ln -sf $(NAME) $(SYMLINK)

show: all
	@if [ -f main.c ]; then \
		$(CC) main.c -L. -lft_malloc -Iincludes -Ilibft -o main_test; \
		export LD_LIBRARY_PATH=. && ./main_test; \
	else \
		echo "main.c not found"; \
	fi

test: all
	@if [ -f test_malloc.c ]; then \
		$(CC) test_malloc.c -L. -lft_malloc -Iincludes -Ilibft -o test_malloc; \
		export LD_LIBRARY_PATH=. && ./test_malloc; \
	else \
		echo "test_malloc.c not found"; \
	fi

clean:
	$(MAKE) clean -C ./libft
	rm -rf $(addprefix $(OBJ_DIR), $(OBJS))
	rm -rf main_test test_malloc

fclean: clean
	$(MAKE) fclean -C ./libft
	rm -rf $(NAME)

re: fclean all

.PHONY: all test test_malloc clean fclean re show
