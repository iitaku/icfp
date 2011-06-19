#ifndef EXPR_HPP
#define EXPR_HPP

#include <map>
#include <string>
#include "mempool.hpp"

#include "tool.hpp"

namespace copy_kawaii {

struct CompileParam;

struct expr {
    enum expr_code{
        APPLY,
        CARD,
        INTEGER,

        /* virtual nodes */
        GENERATE_IMM,
        DIRECT_INT,
        REF_STATIC_VAR,
        GET_SLOT,
        GET_VSLOT,
        CLEAR,
        REF_PREV_VAL
    } code;

    union {
        enum card_code card;
        struct {
            struct expr *f, *a;
        } apply;
        int int_val;
        int slot;

        char *ref_static_var_name;
        char *get_vslot_name;
    }u;

    static expr *
    new_expr(struct expr const &e, MemPool &expr_pool)
    {
        return expr_pool.alloc1(e);
    }

    static struct expr *apply(struct expr *f,
                              struct expr *a,
                              MemPool &p)
    {
        struct expr ret;
        ret.code = APPLY;
        ret.u.apply.f = f;
        ret.u.apply.a = a;
        return new_expr(ret, p);
    }

    static struct expr *card(enum card_code card,
                             MemPool &p)
    {
        struct expr ret;
        ret.code = CARD;
        ret.u.card = card;
        return new_expr(ret,p);
    }

    static struct expr *integer(int val,
                                MemPool &p)
    {
        struct expr ret;
        ret.code = INTEGER;
        ret.u.int_val = val;
        return new_expr(ret,p);
    }

    static struct expr *direct_int(int val,MemPool &p)
    {
        struct expr ret;
        ret.code = DIRECT_INT;
        ret.u.int_val = val;
        return new_expr(ret, p);
    }

    static struct expr *generate_imm(int val, MemPool &p) {
        struct expr ret;
        ret.code = GENERATE_IMM;
        ret.u.int_val = val;
        return new_expr(ret, p);
    }

    static struct expr *ref_static_var(char *var_name, MemPool &p) {
        struct expr ret;
        ret.code = REF_STATIC_VAR;
        ret.u.ref_static_var_name = var_name;
        return new_expr(ret, p);
    }
    static struct expr *get_vslot(char *name, MemPool &p) {
        struct expr ret;
        ret.code = GET_VSLOT;
        ret.u.get_vslot_name = name;
        return new_expr(ret, p);
    }

    static struct expr *get_slot(int slot, MemPool &p) {
        struct expr ret;
        ret.code = GET_SLOT;
        ret.u.slot = slot;
        return new_expr(ret, p);
    }

    static struct expr *clear(MemPool &p) {
        struct expr ret;
        ret.code = CLEAR;
        return new_expr(ret, p);
    }

    static struct expr *ref_prev_val(MemPool &p) {
        struct expr ret;
        ret.code = REF_PREV_VAL;
        return new_expr(ret,p);
    }
};

void dump_expr(expr *e);
expr *parse_expr(const char *src, CompileParam const &cp, MemPool &p);



/*
 * 式中に登場する $xyzzy をおきかえることができる
 * 
 *  var_map["a"] = 4;
 * とかすると
 *  attack $a
 * は
 *  attack 4
 * てなる。
 *
 * 自分で式組み立てるときは、
 *  expr::ref_static_var("a")
 * を使ってね
 */
typedef std::map<std::string, int> var_map_t;

}

#endif
