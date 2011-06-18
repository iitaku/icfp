#include "proponents.hpp"
#include "eval.hpp"

namespace copy_kawaii {

void
zombie_help()
{
    var_map_t vm;

    eval_and_run_at(8, "10000", vm);
    eval_and_run_at(4, "8192", vm);
    eval_and_run_at(6, "3072", vm);

    int e = 0;

    while (1) {
        vm["m0"] = e*2;
        vm["m1"] = e*2 + 1;
        vm["e"] = e;

        eval_and_run_at(129, "attack ($m0)($e)(@4)", vm);
        eval_and_run_at(129, "attack ($m1)($e)(@6)", vm);
        eval_and_run_at(9, "(S (K (help ($m0) ($m1))) (K (@8)))", vm);
        eval_and_run_at(128, "zombie ($e)(@9)", vm);
        eval_and_run_at(9, "clear", vm);

        e = e+1;
    }
}

}
