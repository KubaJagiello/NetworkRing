CC=gcc
FLAGS= -std=c99 -Wall -Werror -g
FILES=node.o queue.o network_helper.o queue_test.o

all: node

node: $(FILES)
	@$(CC) $(FILES) -o $@ -pthread

node.o: node.c node.h
	@$(CC) $(FLAGS) -c $*.c

queue.o: queue.c queue.h
	@$(CC) $(FLAGS) -c $*.c


queue_test.o: queue_test.c

clean:
	@rm -f *.o node
