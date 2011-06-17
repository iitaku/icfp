#include "expr.hpp"
#include "translators.hpp"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

namespace copy_kawaii {

expr *
expand_sk(const expr *src,
          expr *head,
          bool *change)
{
    switch (src->code) {
    case expr::CARD:
        return expr::apply(head, expr::card(src->u.card));

    case expr::APPLY: {
        const expr *src_f = src->u.apply.f;
        const expr *src_a = src->u.apply.a;

        if (head == NULL) {
            if (src_f->code == expr::CARD) {
                head = expr::card(src_f->u.card);
            } else {
                head = expand_sk(src_f, NULL, change);
            }

            return expand_sk(src_a, head, change);
        } else {
            if (src_f->code == expr::CARD) {
                expr *sk = expr::apply(expr::card(CARD_S),
                                       expr::apply(expr::card(CARD_K),
                                                   head));
                sk = expr::apply(sk, expr::card(src_f->u.card));
                return expand_sk(src_a, sk, change);
            } else {
                assert(0);
            }
        }
    }
        break;

    case expr::EMIT_INC_COUNTER: {
        return expr::apply(head, expr::emit_inc_counter(src->u.int_val));
    }
        break;

    case expr::INTEGER:
        assert(0);
        break;

    }

    abort();
    return NULL;
}

}
