CC      = gcc
CFLAGS  = -Wall -Wextra -Werror
SRCS    = main.c drift.c ssrc_check.c helpers.c video_stream.c
OBJS    = $(SRCS:.c=.o)
TARGET  = analyser

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re
