/* -*- tab-width: 4 ; indent-tabs-mode: t -*- */
#include "proponents.hpp"
#include "eval.hpp"
#include "solve.hpp"

namespace copy_kawaii {

void
zombie_help0()
{
    var_map_t vm;

    CriticalHandler ch;

    ch.add_critical_slot("imm");

    eval_and_run_at("10000", vm, CompileParam(RIGHT, 0, 8, false), ch);
    eval_and_run_at("8192", vm, CompileParam(RIGHT, 0, 4, false), ch);
    eval_and_run_at("3072", vm, CompileParam(RIGHT, 0, 6, false), ch);

    int e = 0;

    while (1) {
        vm["m0"] = e*2;
        vm["m1"] = e*2 + 1;
        vm["e"] = e;

        eval_and_run_at("attack ($m0)($e)(@4)", vm, CompileParam(RIGHT, 0, 129, false), ch);
        eval_and_run_at("attack ($m1)($e)(@6)", vm, CompileParam(RIGHT, 0, 129, false), ch);
        eval_and_run_at("(S (K (help ($m0) ($m1))) (K (@8)))", vm, CompileParam(RIGHT, 0, 9, false), ch);
        eval_and_run_at("zombie ($e)(@9)", vm, CompileParam(RIGHT, 0, 129, false), ch);
        eval_and_run_at("clear", vm, CompileParam(RIGHT, 0, 9, false), ch);

        e = e+1;
    }
}

#if 0
void
zombie_help()
{
	var_map_t vm;
	eval_and_run_at("S(K((S((S(K((S(K(S)))K)))S))(K(K))))", vm, CompileParam(RIGHT, 0, 8, false));
	eval_and_run_at("(@8)(@8)(help)(10000)", vm, CompileParam(RIGHT, 0, 4, false));
	eval_and_run_at("(@8)(@8)", vm, CompileParam(RIGHT, 0, 11, false));
	eval_and_run_at("(S(K(@8)))(@11)(attack)(5632)", vm, CompileParam(RIGHT, 0, 3, false));

	eval_and_run_at("zero", vm, CompileParam(RIGHT, 0, 5, false));
	eval_and_run_at("zero", vm, CompileParam(RIGHT, 0, 6, false));
	
	eval_and_run_at("(@3)(@5)", vm, CompileParam(RIGHT, 0, 2, false));
	eval_and_run_at("(@2)(@6)", vm, CompileParam(RIGHT, 0, 129, false));
	eval_and_run_at("succ", vm, CompileParam(LEFT, 0, 6, true));
	eval_and_run_at("(@6)", vm, CompileParam(RIGHT, 0, 2, true));
	
	eval_and_run_at("clear", vm, CompileParam(RIGHT, 0, 6, false));
	eval_and_run_at("zero", vm, CompileParam(RIGHT, 0, 6, false));
	
	while (1) {
		eval_and_run_at("S (K ((@4) (@6)))", vm, CompileParam(RIGHT, 0, 9, false));
		eval_and_run_at("succ", vm, CompileParam(LEFT, 0, 6, true));
		eval_and_run_at("(K (@6))", vm, CompileParam(RIGHT, 0, 9, true));
		eval_and_run_at("zombie (zero)(@9)", vm, CompileParam(RIGHT, 0, 129, false));
		eval_and_run_at("clear", vm, CompileParam(RIGHT, 0, 9, false));
		eval_and_run_at("succ", vm, CompileParam(LEFT, 0, 6, true));
	}
}

void
zombie_help1()
{
    CriticalHandler ch;

	var_map_t vm;
	eval_and_run_at("S(K((S((S(K((S(K(S)))K)))S))(K(K))))", vm, CompileParam(RIGHT, 0, 8, false), ch);
	eval_and_run_at("(@8)(@8)(help)(10000)", vm, CompileParam(RIGHT, 0, 4, false), ch);
	eval_and_run_at("(@8)(@8)", vm, CompileParam(RIGHT, 0, 11, false), ch);
	eval_and_run_at("(S(K(@8)))(@11)(attack)(5632)", vm, CompileParam(RIGHT, 0, 3, false), ch);
	
	eval_and_run_at("zero", vm, CompileParam(RIGHT, 0, 5, false), ch);
	eval_and_run_at("zero", vm, CompileParam(RIGHT, 0, 6, false), ch);
	
	while (1) {
		eval_and_run_at("(@3)(@5)", vm, CompileParam(RIGHT, 0, 2, false), ch);
		
		eval_and_run_at("(@2)(@6)", vm, CompileParam(RIGHT, 0, 129, false));
		eval_and_run_at("S (K ((@4) (@6)))", vm, CompileParam(RIGHT, 0, 9, false));
		eval_and_run_at("succ", vm, CompileParam(LEFT, 0, 6, true));
		
		eval_and_run_at("(@6)", vm, CompileParam(RIGHT, 0, 2, true));
		eval_and_run_at("(K (@6))", vm, CompileParam(RIGHT, 0, 9, true));
		eval_and_run_at("zombie (@5)(@9)", vm, CompileParam(RIGHT, 0, 129, false));
		eval_and_run_at("clear", vm, CompileParam(RIGHT, 0, 9, false));
		
		eval_and_run_at("succ", vm, CompileParam(LEFT, 0, 5, true));
		eval_and_run_at("succ", vm, CompileParam(LEFT, 0, 6, true));
	}
}
#endif


}
