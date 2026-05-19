CC = gcc
CFLAGS = -Wall

all: hello_world

hello_world: hello_world.c
	$(CC) $(CFLAGS) -o hello_world hello_world.c

install: hello_world
	cp hello_world /usr/local/bin/

clean:
	rm -f hello_world
