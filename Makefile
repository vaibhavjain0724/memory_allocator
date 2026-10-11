
CC = clang
CFLAGS = -Wall -Wextra -std=c11
CPPFLAGS = -Iinclude
LDFLAGS = -pthread

SOURCES = src/main.c src/allocator.c src/arena.c
TARGET = allocator

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) include/allocator.h include/arena.h
	$(CC) $(CFLAGS) $(CPPFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) *.o src/*.o
