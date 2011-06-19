#include "expr.hpp"
#include "translators.hpp"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

namespace copy_kawaii {

expr *
expand_sk(const expr *src,
          expr *head,
          bool *change,
          const CompileParam &cp,
          MemPool &p)
{
    switch (src->code) {
    case expr::CARD:
        if (head == NULL) {
            return expr::card(src->u.card, p);
        } else {
            return expr::apply(head, expr::card(src->u.card, p), p);
        }

    case expr::APPLY: {
        const expr *src_f = src->u.apply.f;
        const expr *src_a = src->u.apply.a;

        if (head == NULL) {
            if (src_f->code == expr::CARD) {
                expr *left = expr::card(src_f->u.card, p);
                expr *ret = expr::apply(left,
                                        expand_sk(src_a, NULL, change, cp, p), p);
                return ret;
            } else if (src_a->code == expr::CARD) {
                expr *right = expr::card(src_a->u.card, p);
                expr *ret = expr::apply(expand_sk(src_f, NULL, change, cp, p),
                                        right, p);
                return ret;
            } else {
                head = expand_sk(src_f, NULL, change, cp, p);
            }
            expr *ret = expand_sk(src_a, head, change, cp, p);
            return ret;
        } else {
            if (src_f->code == expr::CARD) {
                expr *sk = expr::apply(expr::card(CARD_S, p),
                                       expr::apply(expr::card(CARD_K, p),
                                                   head,p)
                                       ,p);
                sk = expr::apply(sk, expr::card(src_f->u.card, p), p);
                return expand_sk(src_a, sk, change, cp, p);
            } else {
                assert(0);
            }
        }
    }
        break;

    case expr::GENERATE_IMM: {
        if (head) {
            return expr::apply(head, expr::generate_imm(src->u.int_val, p), p);
        } else {
            return expr::generate_imm(src->u.int_val, p);
        }
    }
        break;

    case expr::DIRECT_INT: {
        return expr::direct_int(src->u.int_val, p);
    }

    case expr::REF_PREV_VAL:
        return expr::ref_prev_val(p);

    case expr::CLEAR:
        return expr::clear(p);

    case expr::GET_VSLOT:
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
