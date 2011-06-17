#include "expr.hpp"
#include "translators.hpp"
#include <stdio.h>
#include <assert.h>

namespace copy_kawaii {

static int
bitcount_shift(int val)
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

static void
emit_bit_shift(commands &dst, int val)
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
            dst.commands.push_back(command(LEFT, 1, CARD_SUCC));
        }

        if (i != 0) {
            dst.commands.push_back(command(LEFT, 1, CARD_DBL));
        }
    }
}

static void
emit_inc_counter(compiler_state &st,
                 struct commands &dst,
                 const expr *src)
{    
    int n = src->u.int_val;
    int d = n - st.val;
    bool emitted = false;
    if (st.val == -1) {
        /* 初期値 */
        d = n;
        dst.commands.push_back(command(RIGHT, 1, CARD_ZERO));
    } else {
        dst.commands.push_back(command(LEFT, 1, CARD_ZERO)); // raise error
        dst.commands.push_back(command(RIGHT, 1, CARD_ZERO));

        if (d < 0) {
            /* カウンタを戻す方法が無いのでエラーを起こす */
            dst.commands.push_back(command(LEFT, 1, CARD_ZERO)); // raise error
            dst.commands.push_back(command(RIGHT, 1, CARD_ZERO));
            d = n;
        } else if (0 && d<bitcount_shift(n)) {
            for (int i=0; i<d; i++) {
                dst.commands.push_back(command(LEFT, 1, CARD_SUCC));
            }
            emitted = true;
        }
    }

    if (!emitted) {
        emit_bit_shift(dst, n);
    }

    st.val = n;

    /* get (succ zero) */
    dst.commands.push_back(command(RIGHT, 0,  CARD_ZERO));
}


static void
do_compile(compiler_state &st,
           struct commands &dst,
           const expr *src)
{
    switch (src->code) {
    case expr::CARD:
        dst.commands.push_back(command(RIGHT, 0, src->u.card));
        break;

    case expr::APPLY: {
        const expr *f = src->u.apply.f;
        const expr *a = src->u.apply.a;

        if (f->code == expr::CARD) {
            compile(st, dst, a);
            dst.commands.push_back(command(LEFT, 0, f->u.card));
        } else if (a->code == expr::CARD) {
            compile(st, dst, f);
            dst.commands.push_back(command(RIGHT, 0, a->u.card));
        } else if (a->code == expr::EMIT_INC_COUNTER) {
            compile(st, dst, f);
            emit_inc_counter(st, dst, a);
        } else {
            fprintf(stderr, "invalid expr");
            assert(0);
        }
    }
        break;

    case expr::EMIT_INC_COUNTER: {
        emit_inc_counter(st, dst, src);
    }
        break;


    case expr::INTEGER: {
        fprintf(stderr, "invalid expr");
        assert(0);
    }
        break;
    }
}

void
compile(compiler_state &st,
        struct commands &dst,
        const expr *src)
{
    do_compile(st, dst, src);
}


}
