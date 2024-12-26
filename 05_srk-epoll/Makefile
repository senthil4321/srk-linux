CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = epoll_demo
SRCS = epoll_demo.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
