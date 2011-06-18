#include "proponents.hpp"
#include "eval.hpp"
#include "solve.hpp"

namespace copy_kawaii {


void
demo_left()
{
    var_map_t vm;

    eval_and_run_at("0", vm, CompileParam(RIGHT, 0, 8, false));
    eval_and_run_at("1000", vm, CompileParam(RIGHT, 0, 9, false));

    while (1) {
        eval_and_run_at("attack (@8)(@8)(@9)", vm, CompileParam(RIGHT, 0, 129, false));
        eval_and_run_at("succ", vm, CompileParam(LEFT, 0, 8, false));

        dump_slots();
    }
}

void
demo_ref_prev_val()
{
    var_map_t vm;

    while (1) {
        eval_and_run_at("S K K", vm, CompileParam(RIGHT, 0, 8, false));
        dump_slots();
        eval_and_run_at("3", vm, CompileParam(RIGHT, 0, 8, true));
        dump_slots();
        eval_and_run_at("clear", vm, CompileParam(LEFT, 0, 8, false));
        dump_slots();
    }
}

}
