

#include <stdlib.h>
#include <memory.h>
#include "message_helper.h"
#define MAX_SIZE 100


char *create_starting_message(char *adress, int port){
    char* message = calloc(MAX_SIZE, sizeof(char));
    strncat(message, "ELECTION\n", 9);
    strncat(message, adress, strlen(adress));
    
}

char *create_election_over_message(int id){

}

char *create_message(){

}

char* get_value_of_id(char *str){

}