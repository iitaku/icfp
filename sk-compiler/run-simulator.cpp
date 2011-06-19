#include "solve.hpp"
#include "command.hpp"
#include "eval.hpp"
#include "hook-program.hpp"

using namespace copy_kawaii;

void init()
{
    for (int i = 0; i < N_SLOTS; i++) {
        pro.push_back(Slot());
        opp.push_back(Slot());
    }

    from_opponent = stdin;
    to_opponent = stdout;
}

int revive_trigger(const event_list_t& event_list, expr* e, var_map_t& vm)
{
    dump_expr(e);
    for (int i=0; i<pro.size(); ++i)
    {
        if (pro[i].v < 0)
        {
            vm["i"] = i;
            return 1;
        }
    }

    return 0;
}

void update_state(commands coms)
{
    for (int i=0; i<coms.size(); i++) {
        apply_card(coms[i].card, coms[i].lr,
                coms[i].slot, true);
        dump_slots();
    }
}

void sim(void)
{
    init();

    {
        CriticalHandler ch;
        var_map_t vm;

        int step = 0;
        
        commands coms;

        hooks.programs.push_back(Hook("revive ($i)", vm, CompileParam(LEFT, 0, 128, false), ch, revive_trigger));
       
        eval_at(coms, "attack (3)(4)(8192)", vm, CompileParam(LEFT, 0, 4, false));
        update_state(coms);
        
        //eval_at(coms, "attack (10)(11)(8192)", vm, CompileParam(LEFT, 0, 4, false));
        //update_state(coms);
    }
}

//#define eval_and_run_at(p, vm, cp, ch) { \
//    eval_at(coms, p, vm, cp);            \
//    update_state(coms);                  \
//}

void
zombie_help()
{ 
    commands coms;

	CriticalHandler ch;
	var_map_t vm;
	event_list_t el;

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
	
	eval_and_run_at("S(K((S((S(K((S(K(S)))K)))S))(K(K))))", vm, CompileParam(RIGHT, imm, n8, false), ch);
	eval_and_run_at("(*n8)(*n8)(help)(10000)", vm, CompileParam(RIGHT, imm, n4, false), ch);
	eval_and_run_at("(*n8)(*n8)", vm, CompileParam(RIGHT, imm, n11, false), ch);
	eval_and_run_at("(S(K(*n8)))(*n11)(attack)(5632)", vm, CompileParam(RIGHT, imm, n3, false), ch);
	
	vsa.free_vslot(n8);
	vsa.free_vslot(n11);
	
	eval_and_run_at("zero", vm, CompileParam(RIGHT, imm, n5, false), ch);
	eval_and_run_at("zero", vm, CompileParam(RIGHT, imm, n6, false), ch);
	
	while (1) {
		eval_and_run_at("(*n3)(*n5)", vm, CompileParam(RIGHT, imm, n2, false), ch);
		
		eval_and_run_at("(*n2)(*n6)", vm, CompileParam(RIGHT, imm, n129, false), ch);
		eval_and_run_at("S (K ((*n4) (*n6)))", vm, CompileParam(RIGHT, imm, n9, false), ch);
		eval_and_run_at("succ", vm, CompileParam(LEFT, imm, n6, true), ch);
		
		eval_and_run_at("(*n6)", vm, CompileParam(RIGHT, imm, n2, true), ch);
		eval_and_run_at("(K (*n6))", vm, CompileParam(RIGHT, imm, n9, true), ch);
		eval_and_run_at("zombie (*n5)(*n9)", vm, CompileParam(RIGHT, imm, n129, false), ch);
		eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, n9, false), ch);
		
		eval_and_run_at("succ", vm, CompileParam(LEFT, imm, n5, true), ch);
		eval_and_run_at("succ", vm, CompileParam(LEFT, imm, n6, true), ch);
		
#if ENABLE_SIM
		for(int i = 255; i >= 0; i--) {
			if(opp[i].v == 1) {
				vm["e"] = 255 - i;
				VSlot dec_op = vsa.alloc_vslot("dec_op");
				
				eval_and_run_at("dec ($e)", vm, CompileParam(RIGHT, imm, dec_op, false), ch);
				
				vsa.free_vslot(dec_op);
			}
		}
#endif
	}
}
int
main(int argc, char **argv)
{
    init();
    zombie_help();
}
