#include "expr.hpp"
#include "translators.hpp"
#include "virtual-slot.hpp"
#include <assert.h>

namespace copy_kawaii {

static expr *
emit_succ(int imm_slot, int val)
{
    if (imm_slot == 0) {
        return expr::generate_imm(val);
    } else if (imm_slot == 1) {
        return expr::apply(expr::card(CARD_SUCC),
                           emit_succ(0, val));
    } else {
        expr *prev = expr::apply(expr::card(CARD_DBL),
                                 emit_succ(imm_slot>>1, val));
        if (imm_slot & 1) {
            return expr::apply(expr::card(CARD_SUCC),
                               prev);
        } else {
            return prev;
        }
    }
}

static expr *
direct_int(int val)
{
    if (val == 0) {
        return expr::card(CARD_ZERO);
    } else if (val == 1) {
        return expr::apply(expr::card(CARD_SUCC),
                           emit_succ(0, val));
    } else {
        expr *prev = expr::apply(expr::card(CARD_DBL),
                                 emit_succ(val>>1, val));
        if (val & 1) {
            return expr::apply(expr::card(CARD_SUCC),
                               prev);
        } else {
            return prev;
        }
    }
}

static expr *
expand_integer(int n, int imm, bool top_level)
{
    if (n == 0) {
        return expr::card(CARD_ZERO);
    } else if (n == 1) {
        return expr::apply(expr::card(CARD_SUCC),
                           expr::card(CARD_ZERO));
    } else if (top_level) {
        return direct_int(n);
    } else {
        if (n <= imm) {
            return direct_int(n);
        } else {
            return expr::apply(expr::card(CARD_GET),
                               emit_succ(imm, n));
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
f(var_map_t &vars, const expr *e, CompileParam const &cp, bool top_level)
{
    switch (e->code) {
    case expr::APPLY:
        return expr::apply(f(vars, e->u.apply.f, cp, false),
                           f(vars, e->u.apply.a, cp, false));

    case expr::CARD:
        return expr::card(e->u.card);

    case expr::INTEGER: {
        int n = e->u.int_val;
        return expand_integer(n, cp.imm_slot, top_level);
    }
        break;

    case expr::REF_STATIC_VAR: {
        int val = vars[e->u.ref_static_var_name];
        return expand_integer(val, cp.imm_slot, top_level);
    }
        break;

    case expr::GET_SLOT:
        return expr::apply(expr::card(CARD_GET),
                           expand_integer(e->u.slot, cp.imm_slot, top_level));

    case expr::CLEAR:
        return expr::clear();

    case expr::REF_PREV_VAL:
        return expr::ref_prev_val();

    case expr::DIRECT_INT:
        return expr::direct_int(e->u.int_val);

    case expr::GET_VSLOT: {
        int vslot = get_vslot(e->u.get_vslot_name);
        return expr::apply(expr::card(CARD_GET),
                           expand_integer(vslot, cp.imm_slot, top_level));
    }

    case expr::GENERATE_IMM: {
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
    return f(vars, src, cp, !cp.ref_prev_val);
}

}
