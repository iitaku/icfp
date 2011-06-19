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
//#define DUEL_WITH_IDIOT_PROGRAM

#if defined(DUEL_IN_LOCAL)
#include <pthread.h>
#endif

namespace copy_kawaii {
#if defined(DUEL_IN_LOCAL)
pthread_mutex_t lock;
volatile int which_player = 0;
__thread int player_no;

__thread FILE *from_opponent;
__thread FILE *to_opponent;
#else
FILE *from_opponent;
FILE *to_opponent;
#endif

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

#if defined(DUEL_IN_LOCAL)
char line[1024*1024];
char line_mode[1024*1024];
char line_slot[1024*1024];
char line_card[1024*1024];
#else
char line[1024*1024];
#endif

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
#if defined(DUEL_WITH_IDIOT_PROGRAM)
    line[0] = '1';
#elif defined(DUEL_IN_LOCAL)
    while(true)
    {
        pthread_mutex_lock(&lock);
        int current_player = which_player;
        pthread_mutex_unlock(&lock);
        if (current_player == player_no) {
            line[0] = line_mode[0];
            break;
        }
    }
#else
    read_line(fp);
#endif

    if (line[0] == '1') {
        ret.com.lr = LEFT;
    } else {
        ret.com.lr = RIGHT;
    }

    if (ret.com.lr == LEFT) {
#if defined(DUEL_WITH_IDIOT_PROGRAM)
        std::pair<std::string, enum card_code> card = std::make_pair("I", get_card_code("I"));
#elif defined(DUEL_IN_LOCAL)
        std::pair<std::string, enum card_code> card = std::make_pair(line_card, get_card_code(line_card));
#else
        std::pair<std::string, enum card_code> card = read_card();
#endif
        ret.com.card = card.second;

#if defined(DUEL_WITH_IDIOT_PROGRAM)
        ret.com.slot = 0;
#elif defined(DUEL_IN_LOCAL)
        ret.com.slot = atoi(line_slot);
#else
        ret.com.slot = read_slot();
#endif

        if (ENABLE_SIM) {
/* Local対戦モードの場合、書き込み側だけが状態を更新する */
#if !defined(DUEL_IN_LOCAL)
            ret.events = apply_card(card.second,
                                    LEFT, ret.com.slot, false);
#endif
        }
    } else {

#if defined(DUEL_IN_LOCAL)
#else
        ret.com.slot = read_slot();
#endif
        
#if defined(DUEL_IN_LOCAL)
        std::pair<std::string, enum card_code> card = std::make_pair(line, get_card_code(line));
#else
        std::pair<std::string, enum card_code> card = read_card();
#endif        

        ret.com.card = card.second;

        if (ENABLE_SIM) {
/* Local対戦モードの場合、書き込み側だけが状態を更新する */
#if !defined(DUEL_IN_LOCAL)
#else
            ret.events = apply_card(card.second,
                                    RIGHT, ret.com.slot, false);
#endif
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
        
#if defined(DUEL_IN_LOCAL)
        sprintf(line_mode, "1\0");
        sprintf(line_card, "%s\0", name);
        sprintf(line_slot, "%d\0", com.slot);
        pthread_mutex_lock(&lock);
        which_player = which_player ^ 1;
        pthread_mutex_unlock(&lock);
#else
        fprintf(to_opponent, "1\n");
        fprintf(to_opponent, "%s\n", name);
        fprintf(to_opponent, "%d\n", com.slot);
        fflush(to_opponent);
#endif

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
#if defined(DUEL_IN_LOCAL)
        sprintf(line_mode, "1\0");
        sprintf(line_card, "%s\0", name);
        sprintf(line_slot, "%d\0", com.slot);
        pthread_mutex_lock(&lock);
        which_player = which_player ^ 1;
        pthread_mutex_unlock(&lock);
#else
        fprintf(to_opponent, "2\n");
        fprintf(to_opponent, "%d\n", com.slot);
        fprintf(to_opponent, "%s\n", name);
        fflush(to_opponent);
#endif

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

    if (ENABLE_SIM) {
        dump_slots();
    }

    return ret;
}

}

