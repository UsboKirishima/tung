TARGET = tung

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

debug: CFLAGS += -g -DDEBUG
debug: clean all

.PHONY: all clean run debug

