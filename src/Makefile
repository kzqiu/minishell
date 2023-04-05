CC 		 = gcc
TARGET   = minishell
C_FILES  = $(wildcard *.c)
OBJS     = $(patsubst %.c,%.o,$(C_FILES))
CFLAGS  = -g -Wall -Werror -pedantic-errors
LDFLAGS =

.PHONY: all clean
all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) $(CFLAGS)
%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	rm -f $(TARGET) $(TARGET).exe $(OBJS)
