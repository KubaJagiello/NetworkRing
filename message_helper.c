#include "message_helper.h"


char *create_message(char *type_of_message, char *additional_info) {
    char* message = allocate_message();
    strncat(message, type_of_message, strlen(type_of_message));
    strncat(message, additional_info, strlen(additional_info));
    return message;
}

char *allocate_message() {
    return calloc(MAX_SIZE, sizeof(char));
}

char *create_election_message(char *type_of_message, char *adress, int port) {
    char id[100];
    char* str_port;
    memset(id, '\0', 100);
    strncat(id, adress, strlen(adress));
    strncat(id, ",", 1);
    str_port = itoa(port);
    strncat(id, str_port, strlen(str_port));
    free(str_port);
    return create_message(type_of_message, id);
}

char* itoa(int i){
    char* string = calloc(MAX_SIZE, sizeof(char));
    sprintf(string, "%d", i);
    return string;
}

char *message_get_id_value(char *message) {
    return strchr(message, '\n');
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