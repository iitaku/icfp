#include "eval.hpp"
#include "expr.hpp"
#include "translators.hpp"
#include "command.hpp"
#include <assert.h>

namespace copy_kawaii {

void
eval_at(enum lr_code lr,
        commands &coms, int slot,
        expr *expr, var_map_t &vm)
{
    bool dump = false;
    struct expr *iexpanded = expand_integer(vm, expr);
    assert(slot != 0);

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
    compile(coms, sk, slot, true);

    if (dump) {
        dump_commands(coms);
    }
}

void
eval_at(enum lr_code lr,
        commands &coms, int slot,
        const char *prog, var_map_t &vm)
{
    expr *e = parse_expr(prog);
    eval_at(lr, coms, slot, e, vm);
}


void
eval_and_run_at(enum lr_code lr,
                int slot,
                const char *prog,
                var_map_t &vm)
{
    commands coms;
    eval_at(lr, coms, slot, prog, vm);

    for (int i=0; i<coms.size(); i++) {
        write_line(coms[i]);
        get_command_line(from_opponent);
    }
}

}
