#ifndef COPY_KAWAII_PROGRAM
#define COPY_KAWAII_PROGRAM

#include <vector>
#include "expr.hpp"

namespace copy_kawaii {

struct stmt {
    enum {
        GOTO,
        EXPR,
        LABEL
    } code ;

    union {
        char *label;
        expr *e;
    }u;

    static stmt goto_(char *label)
    {
        stmt ret;
        ret.code = GOTO;
        ret.u.label = label;
        return ret;
    }
    static stmt label(char *label)
    {
        stmt ret;
        ret.code = LABEL;
        ret.u.label = label;
        return ret;
    }
    static stmt expr(struct expr *e)
    {
        stmt ret;
        ret.code = EXPR;
        ret.u.e = e;
        return ret;
    }

};

struct program {
    std::vector<stmt> stmts;
};

program parse(const char *source,
              int source_len);

void dump_program(program &prog);

void run(program &prog);

}


#endif
