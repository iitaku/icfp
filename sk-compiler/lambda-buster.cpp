/* -*- tab-width: 4 ; indent-tabs-mode: t -*- */

#include "proponents.hpp"
#include "eval.hpp"
#include "solve.hpp"
#include "virtual-slot.hpp"

#define ENABLE_SIM 1

namespace copy_kawaii {

#if 0
bool check_revive_using_vslot(CriticalHandler & ch, var_map_t & vm)
{
	bool execute_revise = false;

	dump_slots();
	for(int i = 0; i < 256; i++) {
		if(pro[i].v == 0){
			if(vsa.states[i] == VirtualSlotAllocator::USED) {
				vm["e"] = i;
				VSlot revise_op = vsa.alloc_vslot("revise_op");
				VSlot imm2 = vsa.alloc_vslot("imm2");

				eval_and_run_at("revive ($e)", vm, CompileParam(RIGHT, imm2, revise_op, false), ch);

				vsa.free_vslot(imm2);
				vsa.free_vslot(revise_op);

				execute_revise = true;
			}
		}
	}

	return execute_revise;
}
#endif

extern bool check_revive_using_vslot(CriticalHandler & ch, var_map_t & vm);

void lambda_buster()
{
	CriticalHandler ch;
	var_map_t vm;
	event_list_t el;

	VSlot imm = vsa.alloc_vslot("imm");

	VSlot n2 = vsa.alloc_vslot("n2");
	VSlot n5 = vsa.alloc_vslot("n5");
	VSlot n3 = vsa.alloc_vslot("n3");
	VSlot n4 = vsa.alloc_vslot("n4");
	VSlot n8 = vsa.alloc_vslot("n8");
	VSlot n12 = vsa.alloc_vslot("n12");
	VSlot n15 = vsa.alloc_vslot("n15");
	VSlot n129 = vsa.alloc_vslot("n129");

	while (1) {
		el = eval_and_run_at("S(K((S((S(K((S(K(S)))K)))S))(K(K))))", vm, CompileParam(RIGHT, imm, n8, false), ch);

		el = eval_and_run_at("S I I (*n8)", vm, CompileParam(RIGHT, imm, n3, false), ch);

		el = eval_and_run_at("(S(K(S(K(S(K(S(((*n3)(help))(10000))(succ)))(dbl)))))(K))", 
			vm, CompileParam(RIGHT, imm, n12, false), ch);
		el = eval_and_run_at("S (zombie) (*n12)", vm, CompileParam(RIGHT, imm, n4, false), ch);

		el = eval_and_run_at("(attack)", vm, CompileParam(RIGHT, imm, n3, true), ch);
		el = eval_and_run_at("(5632)", vm, CompileParam(RIGHT, imm, n3, true), ch);
		el = eval_and_run_at("K", vm, CompileParam(LEFT, imm, n3, true), ch);
		el = eval_and_run_at("S", vm, CompileParam(LEFT, imm, n3, true), ch);

		el = eval_and_run_at("S(S(K((*n3)(succ)))(dbl))(I)", vm, CompileParam(RIGHT, imm, n15, false), ch);

		el = eval_and_run_at("dbl", vm, CompileParam(RIGHT, imm, n3, true), ch);
		el = eval_and_run_at("S", vm, CompileParam(LEFT, imm, n3, true), ch);
		el = eval_and_run_at("I", vm, CompileParam(RIGHT, imm, n3, true), ch);
		el = eval_and_run_at("S", vm, CompileParam(LEFT, imm, n3, true), ch);
		el = eval_and_run_at("(*n15)", vm, CompileParam(RIGHT, imm, n3, true), ch);;

		el = eval_and_run_at("(S (*n3) (*n4))", vm, CompileParam(RIGHT, imm, n2, false), ch);
#if ENABLE_SIM
		bool execute_revise = check_revive_using_vslot(ch, vm);
		if(execute_revise) {
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n2, false), ch);
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n3, false), ch);
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n4, false), ch);
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n8, false), ch);
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n12, false), ch);
			el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n15, false), ch);
		} else {
			break;
		}
#else
		break;
#endif
	}

	el = eval_and_run_at("zero", vm, CompileParam(RIGHT, imm, n5, false), ch);
	int n5i = 0;

	while (1) {		
		if(opp[n5i * 2].v == 10000) {
			el = eval_and_run_at("(*n2)(*n5)", vm, CompileParam(RIGHT, imm, n129, false), ch);
		} else {
			el = eval_and_run_at("(*n3)(*n5)", vm, CompileParam(RIGHT, imm, n129, false), ch);
		}

		n5i++;
		if(n5i > 127) {
			n5i = 0;
			el = eval_and_run_at("put", vm, CompileParam(LEFT, imm, n5, true), ch);
			el = eval_and_run_at("zero", vm, CompileParam(RIGHT, imm, n5, true), ch);
		} else {
			el = eval_and_run_at("succ", vm, CompileParam(LEFT, imm, n5, true), ch);
		}
		
#if ENABLE_SIM
		for(int i = 255; i >= 0; i--) {
			if(opp[i].v == 1) {
				vm["e"] = 255 - i;
				VSlot dec_op = vsa.alloc_vslot("dec_op");

				el = eval_and_run_at("dec ($e)", vm, CompileParam(RIGHT, imm, dec_op, false), ch);

				vsa.free_vslot(dec_op);
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
