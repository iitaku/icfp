#include "expr.hpp"
#include "translators.hpp"
#include <assert.h>

namespace copy_kawaii {

static expr *
expand_integer(int n)
{
    if (n == 0) {
        return expr::card(CARD_ZERO);
    } else if (n == 1) {
        return expr::apply(expr::card(CARD_SUCC),
                           expr::card(CARD_ZERO));
    } else {
        return expr::apply(expr::card(CARD_GET),
                           expr::emit_inc_counter(n));
    }
}

static expr *
f(program &prog, const expr *e)
{
    switch (e->code) {
    case expr::APPLY:
        return expr::apply(f(prog, e->u.apply.f),
                           f(prog, e->u.apply.a));

    case expr::CARD:
        return expr::card(e->u.card);

    case expr::INTEGER: {
        int n = e->u.int_val;
        return expand_integer(n);
    }
        break;

    case expr::REF_STATIC_VAR: {
        int val = prog.vars[e->u.ref_static_var_name];
        return expand_integer(val);
    }
        break;

    case expr::GET_SLOT:
        return expr::apply(expr::card(CARD_GET),
                           expr::apply(expr::card(CARD_GET),
                                       expr::emit_inc_counter(e->u.slot)));

    case expr::EMIT_INC_COUNTER: {
        assert(0);
    }
        break;
    }

    assert(0);
    return NULL;
}

expr *
expand_integer(program &prog, const expr *src)
{
    return f(prog, src);
}

}
