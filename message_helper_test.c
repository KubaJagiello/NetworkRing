#include <assert.h>
#include "message_helper.h"
#include "node.h"


void test_message_election_start();
void test_message_election_over();
void test_message_normal();
void test_message_get_id_value();
void test_message_bools();
void test_substring();
void run_tests_for_message_helper();
void test_message_bigger();
//
//void main(void){
//    test_message_election_start();
//    test_message_election_over();
//    test_message_normal();
//    test_message_get_id_value();
//    test_message_bools();
//    test_substring();
//    test_message_bigger();
//}

void test_message_bigger() {
    assert(first_arg_is_bigger("a", "b") == true);
    assert(first_arg_is_bigger("aa", "ab") == true);
    assert(first_arg_is_bigger("aaa", "aab") == true);
    assert(first_arg_is_bigger("b", "a") == false);
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
    assert(strcmp(message_get_id_value(message_election_start(1337)), "krenim.cs.umu.se,1337") == 0);
}

void test_message_normal() {
    assert(strcmp(message_normal("HelloWorld"), "MESSAGE\nHelloWorld") == 0);
}

void test_message_election_over() {
    assert(strcmp(message_election_over(1337), "ELECTION_OVER\nkrenim.cs.umu.se,1337") == 0);
}

void test_message_election_start() {
    assert(strcmp(message_election_start(1337), "ELECTION\nkrenim.cs.umu.se,1337") == 0);
}

void test_substring(){
    char *first_pointer = "Hejsan";
    char *second_pointer = strchr(first_pointer, 's');
    char substring_array[100];
    memset(substring_array, '\0', 100);
    substring(first_pointer, second_pointer, substring_array);
    assert(strcmp(substring_array, "Hej") == 0);
}