CC = gcc

CFLAGS = -Wall -Wextra -pedantic -std=c11

TARGET = fsh

SRC = fsh.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

install:
	mkdir -p $(DESTDIR)/usr/bin
	cp $(TARGET) $(DESTDIR)/usr/bin/

clean:
	rm -f $(TARGET)

rebuild: clean all

.PHONY: all clean install rebuild
