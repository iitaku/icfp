#include "expr.hpp"
#include "translators.hpp"
#include <assert.h>

namespace copy_kawaii {


static expr *
f(const expr *e)
{
    switch (e->code) {
    case expr::APPLY:
        return expr::apply(f(e->u.apply.f),
                           f(e->u.apply.a));

    case expr::CARD:
        return expr::card(e->u.card);

    case expr::INTEGER: {
        int n = e->u.int_val;
        if (n == 0) {
            return expr::card(CARD_ZERO);
        } else if (n == 1) {
            return expr::apply(expr::card(CARD_SUCC),
                               expr::card(CARD_ZERO));
        } else {
            return expr::apply(expr::card(CARD_GET),
                               expr::apply(expr::card(CARD_SUCC),
                                           expr::emit_inc_counter(n)));
        }
    }
        break;

    case expr::EMIT_INC_COUNTER: {
        assert(0);
    }
        break;
    }

    assert(0);
    return NULL;
}

expr *
expand_integer(const expr *src)
{
    return f(src);
}

}
