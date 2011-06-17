#ifndef TRANSLATORS_HPP
#define TRANSLATORS_HPP

#include <vector>
#include "expr.hpp"
#include "command.hpp"

namespace copy_kawaii {

expr *expand_integer(struct commands &commands,
                     const expr *src);

expr *expand_sk(struct commands &commands,
                const expr *src,
                expr *head,
                bool *change);
void compile(struct commands &commands,
             const expr *src);

}


#endif
