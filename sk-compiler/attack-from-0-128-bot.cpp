#include <stdio.h>

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

enum lr_code {
    LEFT,
    RIGHT
};

FILE *from_opponent;
FILE *to_opponent;

struct line {
    enum lr_code lr;
    int slot;
    int card;
};

char line[1024*1024];

void
getline(FILE *fp)
{
    fgets(line, 1024*1024, fp);
    int len = strlen(line);

    if (line[len-1] == '\n') {
        line[len-1] = '\0';
    }
}

static int
read_slot()
{
    getline(from_opponent);
    int slot = atoi(line);
    return slot;
}

static enum card_code
read_card()
{
    getline(from_opponent);
    return name_to_card[line];
}

struct line
read_line()
{
    struct line l;
    getline(from_opponent);

    if (line[0] == '1') {
        l.lr = LEFT;
    } else {
        l.lr = RIGHT;
    }

    if (l.lr == LEFT) {
        l.card = read_card();
        l.slot = read_slot();
    } else {
        l.slot = read_slot();
        l.card = read_card();
    }
}

struct state {
    enum state_code {
        BUILD_128,              // 0 = 128
        SET_TARGET_ZERO,        // 4 = 0
        BUILD_ATTACK_128,       // 3 = attack 128
        INC_TARGET_N,           // 4++
        BUILD_ATTACK_128_N,     // 2 = attack 128 N
        BUILD_ATTACK_128_N_GET, // 1 = attack 128 N GET
        COPY_8_

        ATTACK
    };

    int current_target;
    int current_counter_value;

    state()
        :current_target(0)
    {}

    
};

int
main(int argc, char **argv)
{
    if (argv[1][0] == '1') {
        read_line();
    }
    while (1) {
        


        struct line l = read_line();
    }
}
