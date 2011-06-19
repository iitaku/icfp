/* -*- tab-width: 4 ; indent-tabs-mode: t -*- */
#include "proponents.hpp"
#include "eval.hpp"
#include "solve.hpp"
#include "virtual-slot.hpp"

#define ENABLE_SIM 1

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

void
zombie_help2()
{
	CriticalHandler ch;
	var_map_t vm;
	event_list_t el;
	
//	  ch.critical_slots["imm"] = true;
	VSlot imm = vsa.alloc_vslot("imm");
	
	VSlot n2 = vsa.alloc_vslot("n2");
	VSlot n3 = vsa.alloc_vslot("n3");
	VSlot n4 = vsa.alloc_vslot("n4");
	VSlot n5 = vsa.alloc_vslot("n5");
	VSlot n6 = vsa.alloc_vslot("n6");
	VSlot n8 = vsa.alloc_vslot("n8");
	VSlot n9 = vsa.alloc_vslot("n9");
	VSlot n11 = vsa.alloc_vslot("n11");
	VSlot n129 = vsa.alloc_vslot("n129");
	
	el = eval_and_run_at("S(K((S((S(K((S(K(S)))K)))S))(K(K))))", vm, CompileParam(RIGHT, imm, n8, false), ch);
	el = eval_and_run_at("(*n8)(*n8)(help)(10000)", vm, CompileParam(RIGHT, imm, n4, false), ch);
	el = eval_and_run_at("(*n8)(*n8)", vm, CompileParam(RIGHT, imm, n11, false), ch);
	el = eval_and_run_at("(S(K(*n8)))(*n11)(attack)(5632)", vm, CompileParam(RIGHT, imm, n3, false), ch);

	el = eval_and_run_at("zero", vm, CompileParam(RIGHT, imm, n5, false), ch);
	el = eval_and_run_at("zero", vm, CompileParam(RIGHT, imm, n6, false), ch);
	
	el = eval_and_run_at("(*n3)(*n5)", vm, CompileParam(RIGHT, imm, n2, false), ch);
	el = eval_and_run_at("(*n2)(*n6)", vm, CompileParam(RIGHT, imm, n129, false), ch);
	el = eval_and_run_at("succ", vm, CompileParam(LEFT, imm, n6, true), ch);
	el = eval_and_run_at("(*n6)", vm, CompileParam(RIGHT, imm, n2, true), ch);
	
	el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n6, false), ch);
	el = eval_and_run_at("zero", vm, CompileParam(RIGHT, imm, n6, false), ch);
	
	while (1) {
		el = eval_and_run_at("S (K ((*n4) (*n6)))", vm, CompileParam(RIGHT, imm, n9, false), ch);
		
		el = eval_and_run_at("succ", vm, CompileParam(LEFT, imm, n6, true), ch);
		el = eval_and_run_at("(K (*n6))", vm, CompileParam(RIGHT, imm, n9, true), ch);
		el = eval_and_run_at("zombie (zero)(*n9)", vm, CompileParam(RIGHT, imm, n129, false), ch);
		el = eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n9, false), ch);
		el = eval_and_run_at("succ", vm, CompileParam(LEFT, imm, n6, true), ch);
	}
}

bool
check_revive_using_vslot(CriticalHandler & ch, var_map_t & vm) {
	
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

void
zombie_help()
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
	VSlot n10 = vsa.alloc_vslot("n10");
	VSlot n11 = vsa.alloc_vslot("n11");
	VSlot n12 = vsa.alloc_vslot("n12");
	VSlot n13 = vsa.alloc_vslot("n13");
	VSlot n14 = vsa.alloc_vslot("n14");
	VSlot n15 = vsa.alloc_vslot("n15");
	VSlot n129 = vsa.alloc_vslot("n129");
	
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
	
	el = eval_and_run_at("zero", vm, CompileParam(RIGHT, imm, n5, false), ch);
	int n5i = 0;
	
	while (1) {
		fprintf(stderr, "checkpoint2\n"); fflush(stderr);
		
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
