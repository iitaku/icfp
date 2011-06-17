#include "expr.hpp"
#include "translators.hpp"
#include <stdio.h>
#include <assert.h>

namespace copy_kawaii {

struct state {
    int val;

    state()
        :val(-1)
    {}
};

static void
emit_inc_counter(state &st,
                 struct commands &dst,
                 const expr *src)
{    
    int n = src->u.int_val;
    int d = n - st.val;
    if (st.val == -1) {
        /* 初期値 */
        d = n;
        dst.commands.push_back(command(RIGHT, 1, CARD_ZERO));
    } else {
        if (d < 0) {
            /* カウンタを戻す方法が無いのでエラーを起こす */
            dst.commands.push_back(command(LEFT, 1, CARD_ZERO)); // raise error
            dst.commands.push_back(command(RIGHT, 1, CARD_ZERO));
            d = n;
        }
    }

    for (int i=0; i<d; i++) {
        dst.commands.push_back(command(LEFT, 1, CARD_SUCC));
    }

    st.val = n;

    /* get (succ zero) */
    dst.commands.push_back(command(RIGHT, 0,  CARD_ZERO));
}


static void
do_compile(state &st,
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
            compile(dst, a);
            dst.commands.push_back(command(LEFT, 0, f->u.card));
        } else if (a->code == expr::CARD) {
            compile(dst, f);
            dst.commands.push_back(command(RIGHT, 0, a->u.card));
        } else if (a->code == expr::EMIT_INC_COUNTER) {
            compile(dst, f);
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
compile(struct commands &dst,
        const expr *src)
{
    state st;
    do_compile(st, dst, src);
}


}
