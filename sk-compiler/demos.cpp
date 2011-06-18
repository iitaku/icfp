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

}
