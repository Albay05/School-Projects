# Compiler and Flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Target file name
TARGET = myFind

# Default rule
all: $(TARGET)

$(TARGET): main.o
	$(CC) $(CFLAGS) main.o -o $(TARGET)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

# Cleaning rule
clean:
	rm -f *.o $(TARGET)