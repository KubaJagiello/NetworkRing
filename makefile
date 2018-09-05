CC=gcc
FLAGS= -std=c99 -Wall -Werror -g
FILES=node.o

all: node

node: $(FILES)
	@$(CC) $(FILES) -o $@ -pthread

node.o: node.c node.h
	@$(CC) $(FLAGS) -c $*.c

clean:
	@rm -f *.o node
