CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
LFLAGS = -pthread -lrt
EXECUTABLES = proj2
DEPENDENCIES = proj2.h
OBJECTS = proj2.o

.PHONY: all clean zipit

all: $(EXECUTABLES)

proj2: $(OBJECTS)
	$(CC) $(CFLAGS) $< -o $@ $(LFLAGS)

proj2.o: proj2.c $(DEPENDENCIES)
	$(CC) $(CFLAGS) $< -c -o $@ $(LFLAGS)

clean:
	rm -f proj2
	rm -f *.o
	rm -f proj2.out
	rm -f proj2.zip

zipit:
	zip proj2.zip *.c *.h Makefile