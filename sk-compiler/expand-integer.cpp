#include "expr.hpp"
#include "translators.hpp"
#include "virtual-slot.hpp"
#include <assert.h>

namespace copy_kawaii {

static expr *
emit_succ(int imm_slot, int val, MemPool &p)
{
    if (imm_slot == 0) {
        return expr::generate_imm(val, p);
    } else if (imm_slot == 1) {
        return expr::apply(expr::card(CARD_SUCC, p),
                           emit_succ(0, val, p), p);
    } else {
        expr *prev = expr::apply(expr::card(CARD_DBL, p),
                                 emit_succ(imm_slot>>1, val, p), p);
        if (imm_slot & 1) {
            return expr::apply(expr::card(CARD_SUCC, p),
                               prev, p);
        } else {
            return prev;
        }
    }
}

static expr *
direct_int(int val, MemPool &p)
{
    if (val == 0) {
        return expr::card(CARD_ZERO, p);
    } else if (val == 1) {
        return expr::apply(expr::card(CARD_SUCC, p),
                           emit_succ(0, val, p), p);
    } else {
        expr *prev = expr::apply(expr::card(CARD_DBL, p),
                                 emit_succ(val>>1, val, p), p);
        if (val & 1) {
            return expr::apply(expr::card(CARD_SUCC, p),
                               prev, p);
        } else {
            return prev;
        }
    }
}

static expr *
expand_integer(int n, int imm, bool top_level, MemPool &p)
{
    if (n == 0) {
        return expr::card(CARD_ZERO, p);
    } else if (n == 1) {
        return expr::apply(expr::card(CARD_SUCC, p),
                           expr::card(CARD_ZERO, p), p);
    } else if (top_level) {
        return direct_int(n, p);
    } else {
        if (n <= imm) {
            return direct_int(n, p);
        } else {
            return expr::apply(expr::card(CARD_GET, p),
                               emit_succ(imm, n, p), p);
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
f(var_map_t &vars, const expr *e, CompileParam const &cp, bool top_level, MemPool &p)
{
    switch (e->code) {
    case expr::APPLY:
        return expr::apply(f(vars, e->u.apply.f, cp, false, p),
                           f(vars, e->u.apply.a, cp, false, p), p);

    case expr::CARD:
        return expr::card(e->u.card, p);

    case expr::INTEGER: {
        int n = e->u.int_val;
        return expand_integer(n, cp.imm_slot, top_level, p);
    }
        break;

    case expr::REF_STATIC_VAR: {
        int val = vars[e->u.ref_static_var_name];
        return expand_integer(val, cp.imm_slot, top_level, p);
    }
        break;

    case expr::GET_SLOT:
        return expr::apply(expr::card(CARD_GET, p),
                           expand_integer(e->u.slot, cp.imm_slot, top_level, p),
            p);

    case expr::CLEAR:
        return expr::clear(p);

    case expr::REF_PREV_VAL:
        return expr::ref_prev_val(p);

    case expr::DIRECT_INT:
        return expr::direct_int(e->u.int_val, p);

    case expr::GET_VSLOT: {
        int vslot = get_vslot(e->u.get_vslot_name);
        return expr::apply(expr::card(CARD_GET, p),
                           expand_integer(vslot, cp.imm_slot, top_level, p), p);
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
               const CompileParam &cp,
               MemPool &p)
{
    return f(vars, src, cp, !cp.ref_prev_val, p);
}

}
