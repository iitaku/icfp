#ifndef TRANSLATORS_HPP
#define TRANSLATORS_HPP

#include <vector>
#include "expr.hpp"
#include "command.hpp"
#include "program.hpp"

namespace copy_kawaii {

expr *expand_integer(program &prog, const expr *src);

expr *expand_sk(const expr *src,
                expr *head,
                bool *change);

struct compiler_state {
    int val;

    compiler_state()
        :val(-1)
    {}
};


void compile(compiler_state &st,
             struct commands &commands,
             const expr *src);

}


#endif
