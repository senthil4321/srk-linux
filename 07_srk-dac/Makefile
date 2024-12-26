CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = dac_example

all: $(TARGET)

$(TARGET): dac_example.c
	$(CC) $(CFLAGS) -o $(TARGET) dac_example.c

clean:
	rm -f $(TARGET)
