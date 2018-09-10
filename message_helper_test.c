#include <assert.h>
#include "message_helper.h"

int main(int argc, char const *argv[]){
    fprintf(stderr, "%s", message_election_start("krenim.cs.umu.se", 1337));
    fprintf(stderr, "\n");
    fprintf(stderr, "%s", message_election_over("krenim.cs.umu.se", 1337));
    fprintf(stderr, "\n");
    fprintf(stderr, "%s", message_normal("text text\n"));
    fprintf(stderr, "%s", message_get_id_value(message_election_start("krenim.cs.umu.se", 1337)));
    assert(message_is_normal("MESSAGE\nHELLO, WORLD! MESSAGEBOIIIIIIIIIIZ\n") == true);
    assert(message_is_normal("MESSAGE111\nHELLO, WORLD! MESSAGEBOIIIIIIIIIIZ\n") == false);
    assert(message_is_election("ELECTION\nHELLO, WORLD! MESSAGEBOIIIIIIIIIIZ\n") == true);
    assert(message_is_election_over("ELECTION_OVER\nHELLO, WORLD! MESSAGEBOIIIIIIIIIIZ\n") == true);
    assert(message_is_election_over("ELECTION_OVER111\nHELLO, WORLD! MESSAGEBOIIIIIIIIIIZ\n") == false);
    assert(message_is_election("ELECTION111\nHELLO, WORLD! MESSAGEBOIIIIIIIIIIZ\n") == false);

}

