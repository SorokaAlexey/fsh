CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
TARGET = fsh
SRC = fsh.c
LIB = libfk10.a  # Name of your static library

all: $(TARGET)

$(TARGET): $(SRC) $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -L. -lfk10

$(LIB):
	$(CC) $(CFLAGS) -c fk10/fk10.c -o fk10.o
	ar rcs $@ fk10.o
	rm -f fk10.o

install:
	mkdir -p $(DESTDIR)/bin
	cp $(TARGET) $(DESTDIR)/bin/

clean:
	rm -f $(TARGET) fk10.o $(LIB)

rebuild: clean all

.PHONY: all clean install rebuild
