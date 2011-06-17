#ifndef EXPR_HPP
#define EXPR_HPP

#include <map>
#include <string>

namespace copy_kawaii {

enum card_code {
    CARD_I,
    CARD_ZERO,
    CARD_SUCC,
    CARD_DBL,
    CARD_GET,
    CARD_PUT,
    CARD_S,
    CARD_K,
    CARD_INC,
    CARD_DEC,
    CARD_ATTACK,
    CARD_HELP,
    CARD_COPY,
    CARD_REVIVE,
    CARD_ZOMBIE
};

typedef std::map<std::string, enum card_code> name_to_card_t;
extern name_to_card_t name_to_card;

const char *get_card_name(enum card_code cc);


struct expr {
    enum expr_code{
        APPLY,
        CARD,
        INTEGER,

        /* virtual nodes */
        EMIT_INC_COUNTER,
    } code;

    union {
        enum card_code card;
        struct {
            struct expr *f, *a;
        } apply;
        int int_val;
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

};

void dump_expr(expr *e);

}

#endif
