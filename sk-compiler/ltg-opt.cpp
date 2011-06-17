#include <stdio.h>
#include <map>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "expr.hpp"
#include "translators.hpp"
#include "command.hpp"
#include "program.hpp"

namespace copy_kawaii {
name_to_card_t name_to_card;

const char *card_to_name_table[] = {
    "I", "zero", "succ", "dbl", "get", "put",
    "S", "K", "inc", "dec", "attack",
    "help", "copy", "revive", "zombie"
};

inline const char *
get_card_name(enum card_code cc)
{
    return card_to_name_table[(int)cc];
}

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

    case expr::REF_STATIC_VAR:
        fprintf(stderr, "$%s ", e->u.ref_static_var_name);
        break;

    }
}

void init()
{
    from_opponent = stdin;
    to_opponent = stdout;

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

    //fprintf(stderr, "%s\n", argv[1]);
    commands coms;

    if (argc >= 2) {
        if (argv[1][0] == '1') {
            get_command_line(from_opponent);
        }
    }

    FILE *in = fopen("prog.txt", "rb");
    if (in == NULL) {
        perror("prog.txt");
        return 1;
    }
    fread(source, 1, sizeof(source), in);
    fclose(in);

    program prog = build(coms, source);
    dump_program(prog);
    run(prog);
/*
    if (argc < 2) {
        build(coms, progs[0]);
        return 0;
    }
*/

}
