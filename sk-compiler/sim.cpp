#include <stdio.h>
#include <map>
#include "optional.hpp"

FILE *log;
FILE *input;
FILE *from_enemy;
FILE *to_enemy;

struct Card {
    int vital;
    Func func;
};

enum card_code {
    CARD_I,
    CARD_ZERO,
    CARD_SUCC,
    CARD_DBL,
    CARD_GET,
    CARD_PUT,
    CARD_S,
    CARD_K,
    CARD_INC,
    CARD_DEC,
    CARD_ATTACK,
    CARD_HELP,
    CARD_COPY,
    CARD_REVIVE,
    CARD_ZOMBIE
};

typedef std::map<std::string, enum card_code> name_to_card_t;
name_to_card_t name_to_card;

static optional<enum card_code>
str_to_card(char *line) {
    
}

char buffer[1024*1024];

struct opponent
{
    FILE *enemy_fp;

    Card cards[256];

    void enemy() {
        int c = fgetc(enemy_fp);
        int read_size;

        if (c == '1') {
            /* apply card to slot */
            char *ret = fgets(buffer, 1024*1024, from_enemy);
            if (ret == NULL) {
                fprintf(log, "error @ 1.card\n");
                return;
            }

            optional<enum card_code card> = str_to_card(buffer);
        } else if (c == '2') {
            /* aplly slot to card */
        } else {
            /* error */
            return;
        }
    }
};


int
main(int argc, char **argv)
{
    log = fopen("log.txt", "wb");
    input = flopen("input.txt", "rb");

    from_enemy = stdin;
    to_enemy = stdout;

    name_to_card["I"] = CARD_I;
    name_to_card["zero"] = CARD_ZERO;
    name_to_card["succ"] = CARD_SUCC;
    name_to_card["dbl"] = CARD_DBL;
    name_to_card["get"] = CARD_GET;
    name_to_card["put"] = CARD_PUT;
    name_to_card["S"] = CARD_S;
    name_to_card["K"] = CARD_K;
    name_to_card["inc"] = CARD_INC;
    name_to_card["dec"] = CARD_DEC;
    name_to_card["attack"] = CARD_ATTACK;
    name_to_card["help"] = CARD_HELP;
    name_to_card["copy"] = CARD_COPY;
    name_to_card["revive"] = CARD_REVIVE;
    name_to_card["zombie"] = CARD_ZOMBIE;

    if (log == 0 || input == 0) {
        perror("error");
        return 1;
    }
}
