#include <stdio.h>
#include <map>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "expr.hpp"
#include "eval.hpp"
#include "translators.hpp"
#include "command.hpp"
#include "program.hpp"
#include "solve.hpp"
#include "proponents.hpp"

namespace copy_kawaii {

void
dump_expr(expr *e)
{
    switch (e->code) {
    case expr::APPLY:
        fprintf(stderr, "(");
        dump_expr(e->u.apply.f);
        dump_expr(e->u.apply.a);
        fprintf(stderr, ") ");
        break;

    case expr::CARD:
        fprintf(stderr, "%s ", get_card_name(e->u.card));
        break;

    case expr::INTEGER:
        fprintf(stderr, "%d ", e->u.int_val);
        break;

    case expr::EMIT_INC_COUNTER:
        fprintf(stderr, "<emit_inc %d> ", e->u.int_val);
        break;

    case expr::REF_PREV_VAL:
        fprintf(stderr, "<prev_val> ");
        break;

    case expr::REF_STATIC_VAR:
        fprintf(stderr, "$%s ", e->u.ref_static_var_name);
        break;

    case expr::GET_SLOT:
        fprintf(stderr, "@%d ", e->u.slot);
        break;
    case expr::GET_VSLOT:
        fprintf(stderr, "*%s ", e->u.get_vslot_name);
        break;

    case expr::CLEAR:
        fprintf(stderr, "clear ");
        break;

    case expr::DIRECT_INT:
        fprintf(stderr, "<direct_int %d> ", e->u.int_val);
        break;
    }
}

void init()
{
    for (int i = 0; i < N_SLOTS; i++) {
        pro.push_back(Slot());
        opp.push_back(Slot(1));
    }

    from_opponent = stdin;
    to_opponent = stdout;
}

program
build(commands &commands,
      const char *source)
{
    program prog = parse(source, strlen(source));
    return prog;
}
}

using namespace copy_kawaii;

char source[1024*1024];

int
main(int argc, char **argv)
{
    init();

    if (argc >= 2) {
        if (argv[1][0] == '1') {
            get_command_line(from_opponent);
        }
    }

    
    var_map_t vm;

    //zombie_help();
    //demo_left();
    //demo_ref_prev_val();
    demo_vslot();
}
