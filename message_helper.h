
#ifndef RING_MESSAGE_HELPER_H
#define RING_MESSAGE_HELPER_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>

#define MAX_SIZE 100
#define ELECTION "ELECTION"
#define ELECTION_OVER "ELECTION_OVER"
#define MESSAGE "MESSAGE"


char* allocate_message();
void int_to_string(int number, char *array_to_fill);
char* create_message(char* type_of_message, char *additional_info);
char* message_election_start(int port);
char* message_normal(char *message);
char* message_election_over(int port);
char* message_get_id_value(char *message);
char* get_my_id(int port);
bool message_is_normal(char* message);
bool message_is_election(char* message);
bool message_is_election_over(char* message);
bool substring_is_equal(char* full_string, char* matching_string);
void substring(char* start_pointer, char* end_pointer, char* array_to_fill);


#endif //RING_MESSAGE_HELPER_H
