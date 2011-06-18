#include "proponents.hpp"
#include "eval.hpp"
#include "solve.hpp"
#include "virtual-slot.hpp"

namespace copy_kawaii {

void
demo_left()
{
    CriticalHandler ch;
    var_map_t vm;

    eval_and_run_at("0", vm, CompileParam(RIGHT, 0, 8, false), ch);
    eval_and_run_at("1000", vm, CompileParam(RIGHT, 0, 9, false), ch);

    while (1) {
        eval_and_run_at("attack (@8)(@8)(@9)", vm, CompileParam(RIGHT, 0, 129, false), ch);
        eval_and_run_at("succ", vm, CompileParam(LEFT, 0, 8, false), ch);

        dump_slots();
    }
}

void
demo_ref_prev_val()
{
    CriticalHandler ch;
    var_map_t vm;

    eval_and_run_at("S K K", vm, CompileParam(RIGHT, 0, 8, false), ch);
    eval_and_run_at("1", vm, CompileParam(RIGHT, 0, 8, false), ch);

    while (1) {
        eval_and_run_at("(S K K)(@6)", vm, CompileParam(RIGHT, 0, 8, false), ch);
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

    VSlot slt = vsa.alloc_vslot("slt", VirtualSlotAllocator::PRI_VITAL);
    VSlot imm = vsa.alloc_vslot("imm", VirtualSlotAllocator::PRI_VITAL);

    eval_and_run_at("30", vm, CompileParam(RIGHT, imm, slt, false), ch);

    vsa.dump();

    while (1) {
        VSlot tmp1 = vsa.alloc_vslot("tmp1");
        VSlot tmp2 = vsa.alloc_vslot("tmp2");
        VSlot tmp3 = vsa.alloc_vslot("tmp3");

        eval_and_run_at("succ *slt", vm, CompileParam(RIGHT, imm, tmp1, false), ch);
        eval_and_run_at("succ (succ *slt)", vm, CompileParam(RIGHT, imm, tmp2, false), ch);
        eval_and_run_at("succ (succ (succ *slt))", vm, CompileParam(RIGHT, imm, tmp3, false), ch);

        VSlot tmp4 = vsa.alloc_vslot("tmp4");
        VSlot tmp5 = vsa.alloc_vslot("tmp5");
        VSlot tmp6 = vsa.alloc_vslot("tmp6");

        eval_and_run_at("*tmp1", vm, CompileParam(RIGHT, imm, tmp4, false), ch);
        eval_and_run_at("*tmp2", vm, CompileParam(RIGHT, imm, tmp5, false), ch);
        eval_and_run_at("*tmp3", vm, CompileParam(RIGHT, imm, tmp6, false), ch);

        vsa.dump();

        eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, tmp1, false), ch);
        eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, tmp2, false), ch);
        eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, tmp3, false), ch);
        eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, tmp4, false), ch);
        eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, tmp5, false), ch);
        eval_and_run_at("clear", vm, CompileParam(RIGHT, imm, tmp6, false), ch);
        vsa.free_vslot(tmp1);
        vsa.free_vslot(tmp2);
        vsa.free_vslot(tmp3);
        vsa.free_vslot(tmp4);
        vsa.free_vslot(tmp5);
        vsa.free_vslot(tmp6);
    }
}


void
demo_robust_zombie()
{
    CriticalHandler ch;
    var_map_t vm;

    ch.critical_slots["imm"] = true;
    ch.critical_slots["v_10000"] = true;
    ch.critical_slots["v_8192"] = true;
    ch.critical_slots["v_3072"] = true;

    VSlot imm = vsa.alloc_vslot("imm");
    VSlot v_10000 = vsa.alloc_vslot("v_10000");
    VSlot v_8192 = vsa.alloc_vslot("v_8192");
    VSlot v_3072 = vsa.alloc_vslot("v_3072");

    eval_and_run_at("10000", vm, CompileParam(RIGHT,imm,v_10000,false), ch);
    eval_and_run_at("8192", vm, CompileParam(RIGHT,imm,v_8192,false), ch);
    eval_and_run_at("3072", vm, CompileParam(RIGHT,imm,v_3072,false), ch);

    int e = 0;

    while (1) {
        vm["e"] = e;
        vm["m0"] = e*2;
        vm["m1"] = e*2 + 1;

#define PARAM(s) CompileParam(RIGHT,imm,s,false)
        VSlot attack = vsa.alloc_vslot("attacker");
        event_list_t el;
        ch.add_critical_slot("attacker");

        el = eval_and_run_at("attack ($m0)($e)(*v_8192)", vm, PARAM(attack), ch);
        if (el.size() != 0) {
            eval_and_run_at("clear", vm, PARAM(attack), ch);
            vsa.free_vslot(attack);
            continue;
        }

        el = eval_and_run_at("attack ($m1)($e)(*v_3072)", vm, PARAM(attack), ch);
        if (el.size() != 0) {
            eval_and_run_at("clear", vm, PARAM(attack), ch);
            vsa.free_vslot(attack);
            continue;
        }

        VSlot zombie_dope = vsa.alloc_vslot("zombie_dope");
        ch.add_critical_slot("zombie_dope");

        el = eval_and_run_at("(S (K (help ($m0) ($m1))) (K (*v_10000)))",
                             vm, CompileParam(RIGHT,imm,zombie_dope,false), ch);

        if (el.size() != 0) {
            eval_and_run_at("clear", vm, PARAM(attack), ch);
            eval_and_run_at("clear", vm, PARAM(zombie_dope), ch);
            vsa.free_vslot(zombie_dope);
            vsa.free_vslot(attack);
            continue;
        }

        el = eval_and_run_at("zombie ($e)(*zombie_dope)", vm, PARAM(attack), ch);
        if (el.size() != 0) {
            eval_and_run_at("clear", vm, PARAM(attack), ch);
            eval_and_run_at("clear", vm, PARAM(zombie_dope), ch);
            vsa.free_vslot(zombie_dope);
            vsa.free_vslot(attack);
            continue;
        }

        eval_and_run_at("clear", vm, PARAM(zombie_dope), ch);

        vsa.free_vslot(zombie_dope);
        vsa.free_vslot(attack);

        e++;
    }
}


void
test_attack0()
{
    CriticalHandler ch;
    var_map_t vm;
    int imm_slot = 0;

    eval_and_run_at("8192", vm, CompileParam(RIGHT, imm_slot, 8, false), ch);

    while (1) {
        for (int i=0; i<255; i++) {
            vm["v"] = i;
            vm["o"] = 255-(i/4);
            eval_and_run_at("attack ($v)($o)(@8)", vm, CompileParam(RIGHT, imm_slot, 129, false), ch);
        }
    }
}


}
