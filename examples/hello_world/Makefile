CC = gcc
CFLAGS = -static -Wall -Wextra -Werror -O2
TARGET = hello
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
