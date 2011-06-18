#include "expr.hpp"
#include "translators.hpp"
#include "slots.hpp"
#include <stdio.h>
#include <assert.h>

namespace copy_kawaii {

#if 0
static int
genint_cost(int val)
{
    int i = 0;
    int max_bitpos = 0;
    int bit_count;
    /* 線形探索ひどい */
    for (i=0; i<24; i++) {
        if (val & (1<<i)) {
            max_bitpos = i;
            bit_count++;
        }
    }

    /* n回シフト + nビット分加算 */
    return max_bitpos + bit_count;
}
#endif

static void
emit_bit_shift(commands &dst, int val, int slot)
{
    int i = 0;
    int max_bitpos = 0;
    /* 線形探索ひどい */
    for (i=0; i<24; i++) {
        if (val & (1<<i)) {
            max_bitpos = i;
        }
    }

    for (int i=max_bitpos; i>=0; i--) {
        if (val & (1<<i)) {
            dst.push_back(command(LEFT, slot, CARD_SUCC));
        }

        if (i != 0) {
            dst.push_back(command(LEFT, slot, CARD_DBL));
        }
    }
}

static void
emit_inc_counter(commands &dst,
                 const expr *src,
                 int prog_slot,
                 bool top_level)
{    
    int n = src->u.int_val;

    dst.push_back(command(LEFT, SLOT_IMM, CARD_PUT));
    dst.push_back(command(RIGHT, SLOT_IMM, CARD_ZERO));
    emit_bit_shift(dst, n, SLOT_IMM);

    /* get (succ zero) */
    dst.push_back(command(RIGHT, prog_slot,  CARD_ZERO));
}

static void
direct_int(commands &dst,
           const expr *src,
           int prog_slot,
           bool top_level)
{
    int n = src->u.int_val;
    dst.push_back(command(LEFT, prog_slot, CARD_PUT)); // slot imm = I
    dst.push_back(command(RIGHT, prog_slot, CARD_ZERO));
    emit_bit_shift(dst, n, prog_slot);
}

static void
do_compile(commands &dst,
           const expr *src,
           int prog_slot,
           bool top_level)
{
    switch (src->code) {
    case expr::CARD:
        dst.push_back(command(RIGHT, prog_slot, src->u.card));
        break;

    case expr::APPLY: {
        const expr *f = src->u.apply.f;
        const expr *a = src->u.apply.a;

        if (f->code == expr::CARD) {
            compile(dst, a, prog_slot, false);
            dst.push_back(command(LEFT, prog_slot, f->u.card));
        } else if (a->code == expr::CARD) {
            compile(dst, f, prog_slot, false);
            dst.push_back(command(RIGHT, prog_slot, a->u.card));
        } else if (a->code == expr::EMIT_INC_COUNTER) {
            compile(dst, f, prog_slot, false);
            emit_inc_counter(dst, a, prog_slot, top_level);
        } else {
            fprintf(stderr, "invalid expr");
            assert(0);
        }
    }
        break;

    case expr::EMIT_INC_COUNTER: {
        emit_inc_counter(dst, src, prog_slot, top_level);
    }
        break;

    case expr::DIRECT_INT: {
        direct_int(dst, src, prog_slot, top_level);
    }
        break;

    case expr::CLEAR:
        dst.push_back(command(LEFT, prog_slot, CARD_PUT));
        break;

    case expr::GET_SLOT:
    case expr::REF_STATIC_VAR:
    case expr::INTEGER: {
        fprintf(stderr, "invalid expr");
        assert(0);
    }
        break;
    }
}

void
compile(commands &dst,
        const expr *src,
        int prog_slot,
        bool top_level)
{
    do_compile(dst, src, prog_slot, top_level);
}


}
