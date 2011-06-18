#include "expr.hpp"
#include "command.hpp"
#include "solve.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define ENABLE_SIM 1

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

struct command
get_command_line(FILE *fp)
{
    struct command l;
    read_line(fp);

    if (line[0] == '1') {
        l.lr = LEFT;
    } else {
        l.lr = RIGHT;
    }

    if (l.lr == LEFT) {
        std::pair<std::string, enum card_code> card = read_card();
        l.card = card.second;
        l.slot = read_slot();

        if (ENABLE_SIM) {
            opp[l.slot].set(card.first, 1);
        }
    } else {
        l.slot = read_slot();
        std::pair<std::string, enum card_code> card = read_card();
        l.card = card.second;

        if (ENABLE_SIM) {
            opp[l.slot].set(card.first, 2);
        }
    }

    return l;
}

void
write_line(command const &com) {
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
            pro[com.slot].set(Card(name), 1);
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
            pro[com.slot].set(Card(name), 2);
        }
    }
#ifdef WITH_SLEEP
    // usleep(100000);
#endif
}

}

