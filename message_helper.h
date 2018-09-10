
#ifndef RING_MESSAGE_HELPER_H
#define RING_MESSAGE_HELPER_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>

#define MAX_SIZE 100
#define ELECTION_START "ELECTION\n"
#define ELECTION_OVER "ELECTION_OVER\n"
#define MESSAGE "MESSAGE\n"


char* allocate_message();
char* itoa(int i);
char* create_message(char* type_of_message, char *additional_info);
char* message_election_start(char *adress, int port);
char* message_normal(char *message);
char* message_election_over(char *adress, int port);
char* message_get_id_value(char *message);

#endif //RING_MESSAGE_HELPER_H
