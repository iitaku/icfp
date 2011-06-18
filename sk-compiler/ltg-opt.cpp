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

    case expr::GET_SLOT:
        fprintf(stderr, "@%d ", e->u.slot);
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
    const char *prog_file = "prog.txt";
    init();

    int opt;

#ifdef PRO_SCRIPT
    prog_file = "pro.txt";
#endif

#ifdef OPP_SCRIPT
    prog_file = "opp.txt";
#endif

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
        case 'p':
            prog_file = optarg;
            break;

        default:
            fprintf(stderr, "usage : ltg-opt [-p prog.txt]\n");
            return 1;
        }
    }

    //fprintf(stderr, "%s\n", argv[1]);
    commands coms;

    if (optind < argc) {
        if (argv[optind][0] == '1') {
            get_command_line(from_opponent);
        }
    }

    FILE *in = fopen(prog_file, "rb");
    if (in == NULL) {
        perror(prog_file);
        return 1;
    }
    fread(source, 1, sizeof(source), in);
    fclose(in);

    program prog = build(coms, source);
    //dump_program(prog);
    run(prog);
/*
    if (argc < 2) {
        build(coms, progs[0]);
        return 0;
    }
*/

}
