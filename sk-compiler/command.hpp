#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <vector>
#include <stdio.h>
#include "expr.hpp"

namespace copy_kawaii {

extern FILE *from_opponent;
extern FILE *to_opponent;

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


struct commands {
    std::vector<command> commands;
};

void dump_commands(commands const &com);
struct command get_command_line(FILE *fp);
void write_line(command const &com);


}


#endif
