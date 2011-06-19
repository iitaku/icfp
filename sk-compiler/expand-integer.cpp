#include "expr.hpp"
#include "translators.hpp"
#include "virtual-slot.hpp"
#include <assert.h>

namespace copy_kawaii {

static expr *
expand_integer(int n, bool top_level)
{
    if (n == 0) {
        return expr::card(CARD_ZERO);
    } else if (n == 1) {
        return expr::apply(expr::card(CARD_SUCC),
                           expr::card(CARD_ZERO));
    } else if (top_level) {
        return expr::direct_int(n);
    } else {
        if (top_level) {
            return expr::apply(expr::card(CARD_GET),
                               expr::emit_inc_counter(n));
        } else {
            return expr::apply(expr::card(CARD_GET),
                               expr::emit_inc_counter(n));
        }
    }
}

static int
get_vslot(const char *vslot_name)
{

#if defined(DUEL_IN_LOCAL)
    return vsa->name_to_slot[vslot_name];
#else
    return vsa.name_to_slot[vslot_name];
#endif

}

static expr *
f(var_map_t &vars, const expr *e, bool top_level)
{
    switch (e->code) {
    case expr::APPLY:
        return expr::apply(f(vars, e->u.apply.f, false),
                           f(vars, e->u.apply.a, false));

    case expr::CARD:
        return expr::card(e->u.card);

    case expr::INTEGER: {
        int n = e->u.int_val;
        return expand_integer(n, top_level);
    }
        break;

    case expr::REF_STATIC_VAR: {
        int val = vars[e->u.ref_static_var_name];
        return expand_integer(val, top_level);
    }
        break;

    case expr::GET_SLOT:
        return expr::apply(expr::card(CARD_GET),
                           expr::apply(expr::card(CARD_GET),
                                       expr::emit_inc_counter(e->u.slot)));

    case expr::CLEAR:
        return expr::clear();

    case expr::REF_PREV_VAL:
        return expr::ref_prev_val();

    case expr::DIRECT_INT:
        return expr::direct_int(e->u.int_val);

    case expr::GET_VSLOT: {
        int vslot = get_vslot(e->u.get_vslot_name);
        return expr::apply(expr::card(CARD_GET),
                           expr::apply(expr::card(CARD_GET),
                                       expr::emit_inc_counter(vslot)));
    }

    case expr::EMIT_INC_COUNTER: {
        assert(0);
    }
        break;
    }

    assert(0);
    return NULL;
}

expr *
expand_integer(var_map_t &vars, const expr *src,
               const CompileParam &cp)
{
    return f(vars, src, true);
}

}
