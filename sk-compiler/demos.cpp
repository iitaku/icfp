#include "proponents.hpp"
#include "eval.hpp"
#include "solve.hpp"

namespace copy_kawaii {


void
demo_left()
{
    var_map_t vm;

    eval_and_run_at("0", vm, CompileParam(RIGHT, 0, 8));
    eval_and_run_at("1000", vm, CompileParam(RIGHT, 0, 9));

    while (1) {
        eval_and_run_at("attack (@8)(@8)(@9)", vm, CompileParam(RIGHT, 0, 129));
        eval_and_run_at("succ", vm, CompileParam(LEFT, 0, 8));

        dump_slots();
    }
}

}
