CC= gcc
CFLAGS= -Wall -lm -Wextra -g

build: tenis.o
	$(CC) tenis.o -lm -o tenis

tenis: tenis.c

clean:
	rm -rf tenis