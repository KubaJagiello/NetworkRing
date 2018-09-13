CC=gcc
FLAGS= -std=c99 -Wall -Werror -g
FILES=node.o queue.o queue_test.o message_helper.o socket_helper.o network_helper.o message_helper_test.o sighant.o

all: node

node: $(FILES)
	@$(CC) $(FILES) -o $@ -pthread

node.o: node.c node.h
	@$(CC) $(FLAGS) -c $*.c

queue.o: queue.c queue.h
	@$(CC) $(FLAGS) -c $*.c

message_helper.o: message_helper.c message_helper.h
	@$(CC) $(FLAGS) -c $*.c

socket_helper.o: socket_helper.c socket_helper.h
	@$(CC) $(FLAGS) -c $*.c

network_helper.o: network_helper.c network_helper.h
	@$(CC) $(FLAGS) -c $*.c

queue_test.o: queue_test.c
	@$(CC) $(FLAGS) -c $*.c

message_helper_test.o: message_helper_test.c
	@$(CC) $(FLAGS) -c $*.c

sighant.o: sighant.c sighant.h
	@$(CC) $(FLAGS) -c $*.c

clean:
	@rm -f *.o node
