/* -*- tab-width: 4 ; indent-tabs-mode: t -*- */

#include "eval.hpp"
#include "virtual-slot.hpp"
#include "solve.hpp"

namespace copy_kawaii {
namespace nakamura {

void
zombie_attack0()
{
	CriticalHandler ch;
	var_map_t vm;
	event_list_t el;

    eval_and_run_at("8192", vm, CompileParam(RIGHT, 0, 1, false), ch);
    eval_and_run_at("255", vm, CompileParam(RIGHT, 0, 2, false), ch);
	eval_and_run_at("attack (3)(@2)(@1)", vm, CompileParam(RIGHT, 0, 128, false), ch);
	eval_and_run_at("attack (4)(@2)(@1)", vm, CompileParam(RIGHT, 0, 128, false), ch);
	eval_and_run_at("clear", vm, CompileParam(RIGHT, 0, 8, false), ch);

	VSlot imm = VSA->alloc_vslot("imm");
	
	VSlot n2 = VSA->alloc_vslot("n2");
	VSlot n5 = VSA->alloc_vslot("n5");
	VSlot n3 = VSA->alloc_vslot("n3");
	VSlot n4 = VSA->alloc_vslot("n4");
	VSlot n8 = VSA->alloc_vslot("n8");
	VSlot n10 = VSA->alloc_vslot("n10");
	VSlot n11 = VSA->alloc_vslot("n11");
	VSlot n12 = VSA->alloc_vslot("n12");
	VSlot n13 = VSA->alloc_vslot("n13");
	VSlot n14 = VSA->alloc_vslot("n14");
	VSlot n15 = VSA->alloc_vslot("n15");
	VSlot n129 = VSA->alloc_vslot("n129");
	
	while (1) {
		el = eval_and_run_at("S(K((S((S(K((S(K(S)))K)))S))(K(K))))", vm, CompileParam(RIGHT, imm, n8, false), ch);
		el = eval_and_run_at("(*n8)(*n8)(help)", vm, CompileParam(RIGHT, imm, n10, false), ch);
		
		el = eval_and_run_at("(S(K(S(K(S(K(S((*n10)(10000))(succ)))(dbl)))))(K))", 
			vm, CompileParam(RIGHT, imm, n12, false), ch);
		el = eval_and_run_at("S (zombie) (*n12)", vm, CompileParam(RIGHT, imm, n4, false), ch);

		el = eval_and_run_at("(*n8)(*n8)", vm, CompileParam(RIGHT, imm, n11, false), ch);
		el = eval_and_run_at("(S(K(*n8)))(*n11)(attack)(5632)", vm, CompileParam(RIGHT, imm, n13, false), ch);
		
		el = eval_and_run_at("(S (K(succ)) (dbl))", vm, CompileParam(RIGHT, imm, n15, false), ch);
		el = eval_and_run_at("(S (*n13) (*n15))", vm, CompileParam(RIGHT, imm, n14, false), ch);
		el = eval_and_run_at("(S (S (*n13) (dbl)) (*n14))", vm, CompileParam(RIGHT, imm, n3, false), ch);

		el = eval_and_run_at("(S (*n3) (*n4))", vm, CompileParam(RIGHT, imm, n2, false), ch);
#if ENABLE_SIM
		bool execute_revise = check_revive_using_vslot(ch, vm);
		if(execute_revise) {
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n8, false), ch);
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n10, false), ch);
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n4, false), ch);
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n11, false), ch);
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n3, false), ch);
		} else {
			break;
		}
#else
		break;
#endif
	}

	el = eval_and_run_at("0", vm, CompileParam(RIGHT, imm, n5, false), ch);
	int n5i = 0;

	while (1) {
		//fprintf(stderr, "checkpoint2\n"); fflush(stderr);
		
		if(opp[n5i * 2].v == 10000) {
			el = eval_and_run_at("(*n2)(*n5)", vm, CompileParam(RIGHT, imm, n129, false), ch);
		} else {
			el = eval_and_run_at("(*n3)(*n5)", vm, CompileParam(RIGHT, imm, n129, false), ch);
		}
		
		n5i++;
		if(n5i > 127) {
			n5i = 0;
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n5, false), ch);
			el = eval_and_run_at("zero", vm, CompileParam(RIGHT, imm, n5, false), ch);
		} else {
			el = eval_and_run_at("succ", vm, CompileParam(LEFT, imm, n5, true), ch);
		}
		
#if ENABLE_SIM
		for(int i = 255; i >= 0; i--) {
			if(opp[i].v == 1) {
				vm["e"] = 255 - i;
				VSlot dec_op = VSA->alloc_vslot("dec_op");
				
				el = eval_and_run_at("dec ($e)", vm, CompileParam(RIGHT, imm, dec_op, false), ch);
				
				VSLOT_FREE(dec_op);
			}
		}
		bool execute_revise = check_revive_using_vslot(ch, vm);
		if(execute_revise) {
			// n129: clear
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n129, false), ch);
			// n5, n6: zero
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n5, false), ch);
			el = eval_and_run_at("zero", vm, CompileParam(RIGHT, imm, n5, false), ch);
		}
#endif
	}
}
}
}
