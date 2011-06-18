#include "proponents.hpp"
#include "eval.hpp"

namespace copy_kawaii {

void
zombie_help()
{
    var_map_t vm;

    eval_and_run_at("10000", vm, CompileParam(RIGHT, 0, 8));
    eval_and_run_at("8192", vm, CompileParam(RIGHT, 0, 4));
    eval_and_run_at("3072", vm, CompileParam(RIGHT, 0, 6));

    int e = 0;

    while (1) {
        vm["m0"] = e*2;
        vm["m1"] = e*2 + 1;
        vm["e"] = e;

        eval_and_run_at("attack ($m0)($e)(@4)", vm, CompileParam(RIGHT, 0, 129));
        eval_and_run_at("attack ($m1)($e)(@6)", vm, CompileParam(RIGHT, 0, 129));
        eval_and_run_at("(S (K (help ($m0) ($m1))) (K (@8)))", vm, CompileParam(RIGHT, 0, 9));
        eval_and_run_at("zombie ($e)(@9)", vm, CompileParam(RIGHT, 0, 129));
        eval_and_run_at("clear", vm, CompileParam(RIGHT, 0, 9));

        e = e+1;
    }
}

}
