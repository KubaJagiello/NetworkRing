#include <assert.h>
#include "message_helper.h"

void test_message_election_start();
void test_message_election_over();
void test_message_normal();
void test_message_get_id_value();
void test_message_bools();
void test_substring();

int main(int argc, char const *argv[]){
    test_message_election_start();
    test_message_election_over();
    test_message_normal();
    test_message_get_id_value();
    test_message_bools();
    test_substring();
}

void test_message_bools(){
    assert(message_is_normal("MESSAGE\nHELLO, WORLD! MESSAGEBOIIIIIIIIIIZ\n") == true);
    assert(message_is_normal("MESSAGE111\nHELLO, WORLD! MESSAGEBOIIIIIIIIIIZ\n") == false);
    assert(message_is_election("ELECTION\nHELLO, WORLD! MESSAGEBOIIIIIIIIIIZ\n") == true);
    assert(message_is_election_over("ELECTION_OVER\nHELLO, WORLD! MESSAGEBOIIIIIIIIIIZ\n") == true);
    assert(message_is_election_over("ELECTION_OVER111\nHELLO, WORLD! MESSAGEBOIIIIIIIIIIZ\n") == false);
    assert(message_is_election("ELECTION111\nHELLO, WORLD! MESSAGEBOIIIIIIIIIIZ\n") == false);
}

void test_message_get_id_value(){
    assert(strcmp(message_get_id_value(message_election_start("krenim.cs.umu.se", 1337)), "krenim.cs.umu.se,1337") == 0);
}

void test_message_normal() {
    assert(strcmp(message_normal("HelloWorld"), "MESSAGE\nHelloWorld") == 0);
}

void test_message_election_over() {
    assert(strcmp(message_election_over("krenim.cs.umu.se", 1337), "ELECTION_OVER\nkrenim.cs.umu.se,1337") == 0);
}

void test_message_election_start() {
    assert(strcmp(message_election_start("krenim.cs.umu.se", 1337), "ELECTION\nkrenim.cs.umu.se,1337") == 0);
}

void test_substring(){
    char *first_pointer = "Hejsan";
    char *second_pointer = strchr(first_pointer, 's');
    char substring_array[100];
    memset(substring_array, '\0', 100);
    substring(first_pointer, second_pointer, substring_array);
    assert(strcmp(substring_array, "Hej") == 0);
}