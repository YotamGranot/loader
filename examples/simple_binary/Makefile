CC = gcc
CFLAGS = -static -nostdlib
TARGET = simple
SRC = nolibc.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
