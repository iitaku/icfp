#ifndef COPY_KAWAII_PROGRAM
#define COPY_KAWAII_PROGRAM

#include <vector>
#include "expr.hpp"

namespace copy_kawaii {

struct static_expr {
    enum expr_code {
        VARREF,
        PLUS,
        MINUS,
        CONST_INT,
    } code;

    union {
        struct {
            static_expr *l, *r;
        } bin;
        char *var_name;
        int val;
    }u;

    static static_expr *bin(enum expr_code code,
                            static_expr *l, static_expr *r)
    {
        static_expr ret;
        ret.code = code;
        ret.u.bin.l = l;
        ret.u.bin.r = r;

        return new static_expr(ret);
    }

    static static_expr *const_int(int val)
    {
        static_expr ret;
        ret.code = CONST_INT;
        ret.u.val = val;
        return new static_expr(ret);
    }

    static static_expr *var_ref(char *name)
    {
        static_expr ret;
        ret.code = VARREF;
        ret.u.var_name = name;
        return new static_expr(ret);
    }
};

struct stmt {
    enum {
        GOTO,
        EXPR,
        LABEL,
        SET_VAR,
    } code ;

    union {
        char *label;
        expr *e;
        struct {
            char *var_name;
            static_expr *expr;
        } set_var;
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

    static stmt set_var(char *var_name,
                        struct static_expr *e) {
        stmt ret;
        ret.code = SET_VAR;
        ret.u.set_var.var_name = var_name;
        ret.u.set_var.expr = e;
        return ret;
    }

};

struct program {
    std::map<std::string, int> vars;
    std::vector<stmt> stmts;
};

program parse(const char *source,
              int source_len);

void dump_program(program &prog);

void run(program &prog);

}


#endif
