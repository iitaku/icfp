#ifndef TRANSLATORS_HPP
#define TRANSLATORS_HPP

#include <vector>
#include "expr.hpp"
#include "command.hpp"
#include "program.hpp"
#include "virtual-slot.hpp"

namespace copy_kawaii {


struct CompileParam {
    enum lr_code ref_prev_lr;
    int imm_slot;
    int prog_slot;
    bool ref_prev_val;

    CompileParam(enum lr_code lr,
                 int imm_slot,
                 int prog_slot,
                 bool ref_prev_val)
        :ref_prev_lr(lr),
         imm_slot(imm_slot),
         prog_slot(prog_slot),
         ref_prev_val(ref_prev_val)
    {}

    CompileParam(enum lr_code lr,
                 const VSlot &imm,
                 const VSlot &prog,
                 bool ref_prev_val)
        :ref_prev_lr(lr),
         imm_slot(imm.slot),
         prog_slot(prog.slot),
         ref_prev_val(ref_prev_val)
    {}
    
    CompileParam()
    {}
};

expr *expand_integer(var_map_t &prog, const expr *src,
                     const CompileParam &cp,
                     MemPool &p);

expr *expand_sk(const expr *src,
                expr *head,
                bool *change,
                CompileParam const &cp,
                MemPool &p);

void compile(commands &commands,
             const expr *src,
             CompileParam const &cp);

}


#endif
