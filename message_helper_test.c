#include "message_helper.h"

int main(int argc, char const *argv[]){
    fprintf(stderr, "%s", message_election_start("krenim.cs.umu.se", 1337));
    fprintf(stderr, "\n");
    fprintf(stderr, "%s", message_election_over("krenim.cs.umu.se", 1337));
    fprintf(stderr, "\n");
    fprintf(stderr, "%s", message_normal("text text\n"));
    fprintf(stderr, "%s", message_get_id_value(message_election_start("krenim.cs.umu.se", 1337)));
}

