CC = gcc
CFLAGS = -Wall -Wextra

OBJS = main.o comment_removal.o file_inclusion.o macro_handler.o

mypreprocessor: $(OBJS)
        $(CC) $(CFLAGS) -o mypreprocessor $(OBJS)

%.o: %.c
        $(CC) $(CFLAGS) -c $< -o $@

clean:
        rm -f *.o mypreprocessor
