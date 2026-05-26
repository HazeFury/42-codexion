NAME        = codexion
CC          = cc
CFLAGS      = -Wall -Wextra -Werror -pthread
OBJS        = $(SRCS:.c=.o)
INCLUDES    = -I.
SRCS        = main.c \
			  src/parsing.c

all: $(NAME)

%.o: %.c Makefile
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

-include $(SRCS:.c=.d)

clean:
	@rm -f $(OBJS) $(SRCS:.c=.d)
	@rm -rf tests/__pycache__ .pytest_cache
	@echo "Object and dependency files cleaned."

fclean: clean
	@rm -f $(NAME)
	@echo "Executable removed."

re: fclean all

test: $(NAME)
	@echo "Lunching all tests..."
	@pytest tests/test_parsing.py -v

.PHONY: all clean fclean re test