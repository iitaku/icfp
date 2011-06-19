#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vector>
#include <stdio.h>
#include "events.hpp"
#include "expr.hpp"

namespace copy_kawaii {

#if defined(DUEL_IN_LOCAL)
extern pthread_mutex_t lock;
extern volatile int which_player;
extern __thread int player_no;
extern __thread FILE *from_opponent;
extern __thread FILE *to_opponent;
#else
extern FILE *from_opponent;
extern FILE *to_opponent;
#endif

enum lr_code {
    LEFT,
    RIGHT
};

struct command {
    enum lr_code lr;
    int slot;
    enum card_code card;

    command(enum lr_code lr,
            int slot,
            enum card_code card)
        :lr(lr), slot(slot), card(card)
    {}
    command(){}
};


typedef std::vector<command> commands;

void dump_commands(commands const &com);

struct command_line {
    struct command com;
    event_list_t events;
};

command_line get_command_line(FILE *fp);
event_list_t write_line(command const &com);


}


#endif
