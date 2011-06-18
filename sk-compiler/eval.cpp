#include "eval.hpp"
#include "expr.hpp"
#include "translators.hpp"
#include "command.hpp"
#include <assert.h>

namespace copy_kawaii {

void
eval_at(commands &coms,
        expr *expr, var_map_t &vm,
        CompileParam const &cp)
{
    bool dump = false;
    struct expr *iexpanded = expand_integer(vm, expr);

    if (dump) {
        fprintf(stderr, "expand integer = ");
        dump_expr(iexpanded);
        fprintf(stderr, "\n");
    }
    bool change = false;
    struct expr *sk = expand_sk(iexpanded, NULL, &change);
    if (dump) {
        fprintf(stderr, "expand sk = ");
        dump_expr(sk);
        fprintf(stderr, "\n");
    }
    compile(coms, sk, cp);

    if (dump) {
        dump_commands(coms);
    }
}

void
eval_at(commands &coms,
        const char *prog, var_map_t &vm,
        CompileParam const &cp)
{
    expr *e = parse_expr(prog);
    eval_at(coms, e, vm, cp);
}


void
eval_and_run_at(const char *prog,
                var_map_t &vm,
                CompileParam const &cp)
{
    commands coms;
    eval_at(coms, prog, vm, cp);

    for (int i=0; i<coms.size(); i++) {
        write_line(coms[i]);
        get_command_line(from_opponent);
    }
}

}
