#ifndef TRANSLATORS_HPP
#define TRANSLATORS_HPP

#include <vector>
#include "expr.hpp"
#include "command.hpp"
#include "program.hpp"

namespace copy_kawaii {

expr *expand_integer(var_map_t &prog, const expr *src);

expr *expand_sk(const expr *src,
                expr *head,
                bool *change);


struct CompileParam {
    enum lr_code innermost_lr;
    int imm_slot;
    int prog_slot;

    CompileParam(enum lr_code lr,
                 int imm_slot,
                 int prog_slot)
        :innermost_lr(lr),
         imm_slot(imm_slot),
         prog_slot(prog_slot)
    {}
};

void compile(commands &commands,
             const expr *src,
             CompileParam const &cp);


}


#endif
