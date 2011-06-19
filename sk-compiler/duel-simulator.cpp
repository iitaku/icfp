#include "solve.hpp"
#include "command.hpp"
#include "eval.hpp"
#include "virtual-slot.hpp"
#include "hook-program.hpp"

using namespace copy_kawaii;

void
zombie_help()
{ 
    commands coms;

	CriticalHandler ch;
	var_map_t vm;
	event_list_t el;

	VSlot imm = VSLOT_ALLOC("imm");
	
	VSlot n2 = VSLOT_ALLOC("n2");
	VSlot n3 = VSLOT_ALLOC("n3");
	VSlot n4 = VSLOT_ALLOC("n4");
	VSlot n5 = VSLOT_ALLOC("n5");
	VSlot n6 = VSLOT_ALLOC("n6");
	VSlot n8 = VSLOT_ALLOC("n8");
	VSlot n9 = VSLOT_ALLOC("n9");
	VSlot n11 = VSLOT_ALLOC("n11");
	VSlot n129 = VSLOT_ALLOC("n129");
	
	eval_and_run_at("S(K((S((S(K((S(K(S)))K)))S))(K(K))))", vm, CompileParam(RIGHT, imm, n8, false), ch);
	eval_and_run_at("(*n8)(*n8)(help)(10000)", vm, CompileParam(RIGHT, imm, n4, false), ch);
	eval_and_run_at("(*n8)(*n8)", vm, CompileParam(RIGHT, imm, n11, false), ch);
	eval_and_run_at("(S(K(*n8)))(*n11)(attack)(5632)", vm, CompileParam(RIGHT, imm, n3, false), ch);
	
	VSLOT_FREE(n8);
	VSLOT_FREE(n11);
	
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
				VSlot dec_op = VSLOT_ALLOC("dec_op");
				
				eval_and_run_at("dec ($e)", vm, CompileParam(RIGHT, imm, dec_op, false), ch);
				
				VSLOT_FREE(dec_op);
			}
		}
#endif
	}
}

void
demo_vslot()
{
    /*
    var_map_t vm;
    eval_and_run_at("@8", vm, CompileParam(RIGHT, 0, 1, false));
    */
    CriticalHandler ch;
    var_map_t vm;

    VSlot slt = VSLOT_ALLOC2("slt", VirtualSlotAllocator::PRI_VITAL);
    VSlot imm = VSLOT_ALLOC2("imm", VirtualSlotAllocator::PRI_VITAL);

    eval_and_run_at("30", vm, CompileParam(RIGHT, imm, slt, false), ch);

    VSLOT_DUMP();

    while (1) {
        VSlot tmp1 = VSLOT_ALLOC("tmp1");
        VSlot tmp2 = VSLOT_ALLOC("tmp2");
        VSlot tmp3 = VSLOT_ALLOC("tmp3");

        eval_and_run_at("succ *slt", vm, CompileParam(RIGHT, imm, tmp1, false), ch);
        eval_and_run_at("succ (succ *slt)", vm, CompileParam(RIGHT, imm, tmp2, false), ch);
        eval_and_run_at("succ (succ (succ *slt))", vm, CompileParam(RIGHT, imm, tmp3, false), ch);

        VSlot tmp4 = VSLOT_ALLOC("tmp4");
        VSlot tmp5 = VSLOT_ALLOC("tmp5");
        VSlot tmp6 = VSLOT_ALLOC("tmp6");

        eval_and_run_at("*tmp1", vm, CompileParam(RIGHT, imm, tmp4, false), ch);
        eval_and_run_at("*tmp2", vm, CompileParam(RIGHT, imm, tmp5, false), ch);
        eval_and_run_at("*tmp3", vm, CompileParam(RIGHT, imm, tmp6, false), ch);

        VSLOT_DUMP();

        eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, tmp1, false), ch);
        eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, tmp2, false), ch);
        eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, tmp3, false), ch);
        eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, tmp4, false), ch);
        eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, tmp5, false), ch);
        eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, tmp6, false), ch);
        VSLOT_FREE(tmp1);
        VSLOT_FREE(tmp2);
        VSLOT_FREE(tmp3);
        VSLOT_FREE(tmp4);
        VSLOT_FREE(tmp5);
        VSLOT_FREE(tmp6);
    }
}

typedef struct
{
    int no;
} Arg;

void* func0(void* arg)
{
    player_no = ((Arg*)arg)->no;
    vsa = &vsa_[player_no];
    
    /* proとoppの領域はtlsではないので、お互い違うところを初期化 */
    for (int i = 0; i < N_SLOTS; i++) {
        pro.push_back(Slot());
    }

    from_opponent = stdin;
    to_opponent = stdout;

    demo_vslot();
}
void* func1(void* arg)
{
    player_no = ((Arg*)arg)->no;
    vsa = &vsa_[player_no];
    
    /* proとoppの領域はtlsではないので、お互い違うところを初期化 */
    for (int i = 0; i < N_SLOTS; i++) {
        opp.push_back(Slot());
    }

    from_opponent = stdin;
    to_opponent = stdout;

    zombie_help();
}

int
main(int argc, char **argv)
{
    pthread_mutex_init(&lock, NULL);
    
    pthread_t th[2];
    Arg arg[2];
    arg[0].no = 0;
    arg[1].no = 1;

    pthread_create(&th[0], NULL, func0, &arg[0]);
    pthread_create(&th[1], NULL, func1, &arg[1]);

    pthread_join(th[0], NULL);
    pthread_join(th[1], NULL);
    
    return 0;
}
