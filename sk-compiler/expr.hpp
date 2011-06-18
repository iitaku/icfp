#ifndef EXPR_HPP
#define EXPR_HPP

#include <map>
#include <string>

#include "tool.hpp"

namespace copy_kawaii {

struct expr {
    enum expr_code{
        APPLY,
        CARD,
        INTEGER,

        /* virtual nodes */
        EMIT_INC_COUNTER,
        REF_STATIC_VAR,
    } code;

    union {
        enum card_code card;
        struct {
            struct expr *f, *a;
        } apply;
        int int_val;

        char *ref_static_var_name;
    }u;

    static struct expr *apply(struct expr *f,
                             struct expr *a)
    {
        struct expr ret;
        ret.code = APPLY;
        ret.u.apply.f = f;
        ret.u.apply.a = a;
        return new expr(ret);
    }

    static struct expr *card(enum card_code card)
    {
        struct expr ret;
        ret.code = CARD;
        ret.u.card = card;
        return new expr(ret);
    }

    static struct expr *integer(int val) {
        struct expr ret;
        ret.code = INTEGER;
        ret.u.int_val = val;
        return new expr(ret);
    }

    static struct expr *emit_inc_counter(int val) {
        struct expr ret;
        ret.code = EMIT_INC_COUNTER;
        ret.u.int_val = val;
        return new expr(ret);
    }

    static struct expr *ref_static_var(char *var_name) {
        struct expr ret;
        ret.code = REF_STATIC_VAR;
        ret.u.ref_static_var_name = var_name;
        return new expr(ret);
    }
};

void dump_expr(expr *e);

}

#endif
