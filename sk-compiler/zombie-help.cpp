#include "proponents.hpp"
#include "eval.hpp"
#include "solve.hpp"

namespace copy_kawaii {

void
zombie_help0()
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

#if 0
void
zombie_help2()
{
	var_map_t vm;
	eval_and_run_at(8, "S(K((S((S(K((S(K(S)))K)))S))(K(K))))", vm);
	eval_and_run_at(4, "(@8)(@8)(help)(10000)", vm);
	eval_and_run_at(11, "(@8)(@8)", vm);
	eval_and_run_at(3, "(S(K(@8)))(@11)(attack)(5632)", vm);

	int e = 0;
	
	vm["m0"] = e*2;
	vm["m1"] = e*2 + 1;
	vm["e"] = e;
	
	eval_and_run_at(2, "(@3)($e)", vm);
	
	eval_and_run_at(129, "(@2)($m0)", vm);
	eval_and_run_at(129, "(@2)($m1)", vm);
	
	while (1) {
		vm["m0"] = e*2;
		vm["m1"] = e*2 + 1;
		
		eval_and_run_at(9, "(S (K ((@4) ($m0))) (K ($m1)))", vm);
		eval_and_run_at(128, "zombie ($e)(@9)", vm);
		eval_and_run_at(9, "clear", vm);
		eval_and_run_at(2, "clear", vm);

		e = e+1;
	}
}
#endif

void
zombie_help()
{
	var_map_t vm;
	eval_and_run_at("S(K((S((S(K((S(K(S)))K)))S))(K(K))))", vm, CompileParam(RIGHT, 0, 8));
	eval_and_run_at("(@8)(@8)(help)(10000)", vm, CompileParam(RIGHT, 0, 4));
	eval_and_run_at("(@8)(@8)", vm, CompileParam(RIGHT, 0, 11));
	eval_and_run_at("(S(K(@8)))(@11)(attack)(5632)", vm, CompileParam(RIGHT, 0, 3));
	
	int e = 0;
	
	while (1) {
		vm["m0"] = e*2;
		vm["m1"] = e*2 + 1;
		vm["e"] = e;
		
		eval_and_run_at("(@3)($e)", vm, CompileParam(RIGHT, 0, 2));
		
		eval_and_run_at("(@2)($m0)", vm, CompileParam(RIGHT, 0, 129));
		eval_and_run_at("($m1)", vm, CompileParam(RIGHT, 0, 2));
		eval_and_run_at("(S (K ((@4) ($m0))) (K ($m1)))", vm, CompileParam(RIGHT, 0, 9));
		eval_and_run_at("zombie ($e)(@9)", vm, CompileParam(RIGHT, 0, 129));
		eval_and_run_at("clear", vm, CompileParam(RIGHT, 0, 9));
		
		e = e+1;
		
	}
}


}
