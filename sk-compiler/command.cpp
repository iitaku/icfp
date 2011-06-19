#include "expr.hpp"
#include "command.hpp"
#include "solve.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define ENABLE_SIM 1
//#define ENABLE_SIM 0

namespace copy_kawaii {
FILE *from_opponent;
FILE *to_opponent;

void
dump_commands(commands const &com)
{
    int n = com.size();
    for (int i=0; i<n; i++) {
        if (com[i].lr == LEFT) {
            fprintf(stderr, "left slot=%d card=%s\n",
                    com[i].slot,
                    get_card_name(com[i].card));
        } else {
            fprintf(stderr, "right slot=%d card=%s\n",
                    com[i].slot,
                    get_card_name(com[i].card));
        }
    }
}

char line[1024*1024];

void
read_line(FILE *fp)
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
    read_line(from_opponent);
    int slot = atoi(line);
    return slot;
}

static std::pair<std::string, enum card_code>
read_card()
{
    read_line(from_opponent);
    return std::make_pair(line, get_card_code(line));
}

struct command_line
get_command_line(FILE *fp)
{
    command_line ret;
#ifdef DUEL_WITH_IDIOT_PROGRAM
    line[0] = '1';
#else
    read_line(fp);
#endif

    if (line[0] == '1') {
        ret.com.lr = LEFT;
    } else {
        ret.com.lr = RIGHT;
    }


    if (ret.com.lr == LEFT) {
#ifdef DUEL_WITH_IDIOT_PROGRAM        
        std::pair<std::string, enum card_code> card = std::make_pair("I", get_card_code("I"));
#else
        std::pair<std::string, enum card_code> card = read_card();
#endif
        ret.com.card = card.second;

#ifdef DUEL_WITH_IDIOT_PROGRAM        
        ret.com.slot = 0;
#else
        ret.com.slot = read_slot();
#endif

        if (ENABLE_SIM) {
            ret.events = apply_card(card.second,
                                    LEFT, ret.com.slot, false);
        }
    } else {
        ret.com.slot = read_slot();
        std::pair<std::string, enum card_code> card = read_card();
        ret.com.card = card.second;

        if (ENABLE_SIM) {
            ret.events = apply_card(card.second,
                                    RIGHT, ret.com.slot, false);
        }
    }

    return ret;
}

event_list_t
write_line(command const &com) {
    event_list_t ret;
    bool dump = false;
    const char *name = get_card_name(com.card);

    assert(com.slot < 256);

    if (com.lr == LEFT) {
        fprintf(to_opponent, "1\n");
        fprintf(to_opponent, "%s\n", name);
        fprintf(to_opponent, "%d\n", com.slot);
        fflush(to_opponent);

        if (dump) {
            fprintf(stderr, "1\n");
            fprintf(stderr, "%s\n", name);
            fprintf(stderr, "%d\n", com.slot);
        }
        fflush(stderr);

        if (ENABLE_SIM) {
            ret = apply_card(com.card, LEFT, com.slot, true);
        }
    } else {
        fprintf(to_opponent, "2\n");
        fprintf(to_opponent, "%d\n", com.slot);
        fprintf(to_opponent, "%s\n", name);
        fflush(to_opponent);

        if (dump) {
            fprintf(stderr, "2\n");
            fprintf(stderr, "%d\n", com.slot);
            fprintf(stderr, "%s\n", name);
        }
        fflush(stderr);

        if (ENABLE_SIM) {
            ret = apply_card(com.card, RIGHT, com.slot, true);
        }
    }
#ifdef WITH_SLEEP
    // usleep(100000);
#endif

    return ret;
}

}

