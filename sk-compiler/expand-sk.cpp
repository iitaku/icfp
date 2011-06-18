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
        if (head == NULL) {
            return expr::card(src->u.card);
        } else {
            return expr::apply(head, expr::card(src->u.card));
        }

    case expr::APPLY: {
        const expr *src_f = src->u.apply.f;
        const expr *src_a = src->u.apply.a;

        if (head == NULL) {
#if 0
            if (src_f->code == expr::CARD) {
                expr *left = expr::card(src_f->u.card);
                return expr::apply(left,
                                   expand_sk(src_a, NULL, change));
            } else if (src_a->code == expr::CARD) {
                expr *right = expr::card(src_a->u.card);
                return expr::apply(expand_sk(src_f, NULL, change),
                                   right);
            } else {
                expr *head = expand_sk(src_f, NULL, change);
                return expand_sk(src_a, head, change);
            }
#endif
            if (src_f->code == expr::CARD) {
                expr *left = expr::card(src_f->u.card);
                expr *ret = expr::apply(left,
                                        expand_sk(src_a, NULL, change));
                return ret;
            } else if (src_a->code == expr::CARD) {
                expr *right = expr::card(src_a->u.card);
                expr *ret = expr::apply(expand_sk(src_f, NULL, change),
                                        right);
                return ret;
            } else {
                head = expand_sk(src_f, NULL, change);
            }
            expr *ret = expand_sk(src_a, head, change);
            return ret;
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
        if (head) {
            return expr::apply(head, expr::emit_inc_counter(src->u.int_val));
        } else {
            return expr::emit_inc_counter(src->u.int_val);
        }
    }
        break;

    case expr::GET_SLOT:
    case expr::REF_STATIC_VAR:
    case expr::INTEGER:
        assert(0);
        break;

    }

    abort();
    return NULL;
}

}
