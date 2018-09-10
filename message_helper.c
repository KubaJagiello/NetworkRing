#include "message_helper.h"

char *create_message(char *type_of_message, char *additional_info) {
    char* message = allocate_message();
    strncat(message, type_of_message, strlen(type_of_message));
    strncat(message, "\n", 1);
    strncat(message, additional_info, strlen(additional_info));
    return message;
}

char *allocate_message() {
    return calloc(MAX_SIZE, sizeof(char));
}

char *create_election_message(char *type_of_message, char *adress, int port) {
    char id[100];
    char string_port[MAX_SIZE];
    memset(id, '\0', MAX_SIZE);
    memset(string_port, '\0', MAX_SIZE);
    strncat(id, adress, strlen(adress));
    strncat(id, ",", 1);
    int_to_string(port, string_port);
    strncat(id, string_port, strlen(string_port));
    return create_message(type_of_message, id);
}

void int_to_string(int number, char *array_to_fill){
    sprintf(array_to_fill, "%d", number);
}

char *message_get_id_value(char *message) {
    char* temp = strchr(message, '\n');
    return ++temp;
}

char *message_election_over(char *adress, int port) {
    return create_election_message(ELECTION_OVER, adress, port);
}

char *message_normal(char *message) {
    return create_message(MESSAGE, message);
}

char *message_election_start(char *adress, int port) {
    return create_election_message(ELECTION_START, adress, port);
}

bool message_is_normal(char *message) {
    return substring_is_equal(message, MESSAGE);
}

bool message_is_election(char *message) {
    return substring_is_equal(message, ELECTION_START);
}

bool message_is_election_over(char *message) {
    return substring_is_equal(message, ELECTION_OVER);
}

void substring(char *start_pointer, char *end_pointer, char* array_to_fill) {
    while(start_pointer != end_pointer)
        *array_to_fill++ = *start_pointer++;
}

bool substring_is_equal(char *full_string, char *matching_string) {
    char* end_position = strchr(full_string, '\n');
    if(end_position == NULL)
        return false;
    char sub_string[MAX_SIZE];
    memset(sub_string, '\0', MAX_SIZE);
    substring(full_string, end_position, sub_string);
    return strcmp(sub_string, matching_string) == 0;
}
