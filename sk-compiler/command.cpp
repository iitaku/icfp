#include "command.hpp"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

namespace copy_kawaii {
FILE *from_opponent;
FILE *to_opponent;

void
dump_commands(commands const &com)
{
    int n = com.commands.size();
    for (int i=0; i<n; i++) {
        if (com.commands[i].lr == LEFT) {
            fprintf(stderr, "left slot=%d card=%s\n",
                    com.commands[i].slot,
                    get_card_name(com.commands[i].card));
        } else {
            fprintf(stderr, "right slot=%d card=%s\n",
                    com.commands[i].slot,
                    get_card_name(com.commands[i].card));
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

static enum card_code
read_card()
{
    read_line(from_opponent);
    return name_to_card[line];
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
        l.card = read_card();
        l.slot = read_slot();
    } else {
        l.slot = read_slot();
        l.card = read_card();
    }

    return l;
}

void
write_line(command const &com) {
    bool dump = 1;

    if (com.lr == LEFT) {
        fprintf(to_opponent, "1\n");
        fprintf(to_opponent, "%s\n", get_card_name(com.card));
        fprintf(to_opponent, "%d\n", com.slot);
        fflush(to_opponent);

        if (dump) {
            fprintf(stderr, "1\n");
            fprintf(stderr, "%s\n", get_card_name(com.card));
            fprintf(stderr, "%d\n", com.slot);
        }
        fflush(stderr);
    } else {
        fprintf(to_opponent, "2\n");
        fprintf(to_opponent, "%d\n", com.slot);
        fprintf(to_opponent, "%s\n", get_card_name(com.card));
        fflush(to_opponent);

        if (dump) {
            fprintf(stderr, "2\n");
            fprintf(stderr, "%d\n", com.slot);
            fprintf(stderr, "%s\n", get_card_name(com.card));
        }
        fflush(stderr);
    }
    sleep(1);
}

}

