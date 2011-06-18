#include "tool.hpp"
#include "program.hpp"
#include "translators.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

namespace copy_kawaii {

struct lexval {
    enum lcode {
        EOL,
        EOF_,
        LPAR,
        RPAR,
        CARD,
        DIGIT,
        LABEL,
        GOTO,
        SET,
        SYM,
        END,
        GOTO_IF,
        PLUS,
        MINUS,
        MUL,
        DIV,
        EQ,
        EQEQ,
        NE,
        LT,
        GT,
        LE,
        GE,
        EXCLAM,
        DOLLER
    } code;

    union {
        enum card_code card;
        int digit_val;
        char *sym;
    } u;

    lexval(enum lcode lc)
        :code(lc){}

    lexval(enum card_code card)
        :code(CARD)
    {
        u.card = card;
    }

    static lexval digit(int val) {
        lexval ret(DIGIT);
        ret.u.digit_val = val;
        return ret;
    }

    static lexval symval(char *sym) {
        lexval ret(SYM);
        ret.u.sym = sym;
        return ret;
    }
};

struct lexer {
    const char *buf;
    int len;

    int cur;
    bool has_last;
    int last;
    lexval lastval;

    lexer(const char *buf,
          int linelen)
        :buf(buf),
         len(linelen),
         cur(0),
         has_last(false),
         last(0),
         lastval(lexval::EOL)
    {}
};

static int
lgetc(struct lexer *l)
{
    if (l->has_last) {
        l->has_last = false;
        return l->last;
    }

    if (l->cur == l->len) {
        return -1;
    }

    return l->buf[l->cur++];
}

static void
lungetc(struct lexer *l, int c )
{
    l->has_last = true;
    l->last = c;
}

struct lexval
lex(struct lexer *l)
{
    char symbuf[256];
    int sympos;
    while (1) {
        int c = lgetc(l);
        if (c == -1) {
            return l->lastval = lexval(lexval::EOF_);
        }
        switch (c) {
        case ' ':
            continue;

        case '\n':
            return l->lastval = lexval(lexval::EOL);

        case '(':
            return l->lastval = lexval(lexval::LPAR);
        case ')':
            return l->lastval = lexval(lexval::RPAR);

        default:
            if (isalpha(c) || c == '_') {
                symbuf[0] = c;
                sympos = 1;
                while (1) {
                    c = lgetc(l);
                    if (! (isalnum(c)
                           || (c == '_')))
                    {
                        lungetc(l, c);

                        symbuf[sympos] = '\0';
                        enum card_code code = get_card_code(symbuf);
                        if (CARD_UNKNOWN == code) {
                            if (strcmp("goto", symbuf) == 0) {
                                return l->lastval = lexval(lexval::GOTO);
                            }
                            if (strcmp("goto_if", symbuf) == 0) {
                                return l->lastval = lexval(lexval::GOTO_IF);
                            }
                            if (strcmp("label", symbuf) == 0) {
                                return l->lastval = lexval(lexval::LABEL);
                            }
                            if (strcmp("end", symbuf) == 0) {
                                return l->lastval = lexval(lexval::END);
                            }
                            if (strcmp("set", symbuf) == 0) {
                                return l->lastval = lexval(lexval::SET);
                            }

                            return l->lastval = lexval::symval(strdup(symbuf));

                            fprintf(stderr, "unknown card name '%s'\n", symbuf);
                            abort();
                        }

                        return l->lastval = lexval(code);
                    }

                    symbuf[sympos++] = c;
                }
            } else if (isdigit(c)) {
                symbuf[0] = c;
                sympos = 1;
                while (1) {
                    c = lgetc(l);
                    if (! isdigit(c)) {
                        lungetc(l, c);

                        symbuf[sympos] = '\0';
                        return l->lastval = lexval::digit(atoi(symbuf));
                    }

                    symbuf[sympos++] = c;
                }
            }

            switch (c) {
            case '*':
                return l->lastval = lexval(lexval::MUL);
            case '/':
                return l->lastval = lexval(lexval::DIV);
            case '+':
                return l->lastval = lexval(lexval::PLUS);
            case '<': {
                int c = lgetc(l);
                if (c == '=') {
                    return l->lastval = lexval(lexval::LE);
                } else {
                    lungetc(l, c);
                    return l->lastval = lexval(lexval::LT);
                }
            }
                break;

            case '>': {
                int c = lgetc(l);
                if (c == '=') {
                    return l->lastval = lexval(lexval::GE);
                } else {
                    lungetc(l, c);
                    return l->lastval = lexval(lexval::GT);
                }
            }
                break;

            case '!': {
                int c = lgetc(l);
                if (c == '=') {
                    return l->lastval = lexval(lexval::NE);
                } else {
                    lungetc(l, c);
                    return l->lastval = lexval(lexval::EXCLAM);
                }
            }
                break;

            case '=': {
                int c = lgetc(l);
                if (c == '=') {
                    return l->lastval = lexval(lexval::EQEQ);
                } else {
                    lungetc(l, c);
                    return l->lastval = lexval(lexval::EQ);
                }
            }
                break;

            case '-':
                return l->lastval = lexval(lexval::MINUS);
            case '$':
                return l->lastval = lexval(lexval::DOLLER);

            default:
                break;
            }

            fprintf(stderr, "read unknown charactor = %c\n", c);
            abort();
        }
    }
}


static struct expr *
parse_expr(struct lexer *l,
           bool toplev)
{
    expr *prev = NULL, *cur = NULL;
    while (1) {
        switch (l->lastval.code) {
        case lexval::LPAR:
            lex(l);
            cur = parse_expr(l, false);
            if (prev == NULL) {
                prev = cur;
            } else {
                prev = expr::apply(prev, cur);
            }
            break;

        case lexval::RPAR:
            lex(l);
            if (toplev) {
                fprintf(stderr, "unmatch ();\n");
                abort();
            }
            if (prev == NULL) {
                fprintf(stderr, "empty expr\n");
                abort();
            }
            return prev;

        case lexval::CARD:
            cur = expr::card(l->lastval.u.card);
            lex(l);
            if (prev == NULL) {
                prev = cur;
            } else {
                prev = expr::apply(prev, cur);
            }
            break;

        case lexval::DIGIT:
            cur = expr::integer(l->lastval.u.digit_val);
            lex(l);
            if (prev == NULL) {
                prev = cur;
            } else {
                prev = expr::apply(prev, cur);
            }
            break;

        case lexval::DOLLER:
            lex(l);
            if (l->lastval.code != lexval::SYM) {
                fprintf(stderr, "error near '$'\n");
                assert(0);
            }
            cur = expr::ref_static_var(l->lastval.u.sym);
            lex(l);
            if (prev == NULL) {
                prev = cur;
            } else {
                prev = expr::apply(prev, cur);
            }
            break;

        case lexval::EOL:
            if (! toplev) {
                fprintf(stderr, "unmatch ();\n");
                abort();
            }

            if (prev == NULL) {
                fprintf(stderr, "empty expr\n");
                abort();
            }

            return prev;

        default:
            fprintf(stderr, "??\n");
            assert(0);
        }
    }
}


static struct expr *
parse_expr(lexer &l)
{
    return parse_expr(&l, true);
}

static static_expr *parse_static_expr(lexer &l,
                                      bool is_toplev);

static static_expr *
static_term(lexer &l)
{
    static_expr *e;
    switch (l.lastval.code) {
    case lexval::DIGIT:
        e = static_expr::const_int(l.lastval.u.digit_val);
        lex(&l);
        return e;

    case lexval::DOLLER:
        lex(&l);
        if (l.lastval.code != lexval::SYM) {
            fprintf(stderr, "unexpected token near $\n");
            assert(0);
        }
        e = static_expr::var_ref(l.lastval.u.sym);
        lex(&l);
        return e;

    case lexval::LPAR:
        lex(&l);
        e = parse_static_expr(l, false);
        if (l.lastval.code != lexval::RPAR) {
            fprintf(stderr, "unmatch ()\n");
            assert(0);
        }
        lex(&l);
        return e;

    default:
        break;
    }

    fprintf(stderr, "expr error \n");
    assert(0);
}

static static_expr *
parse_static_expr(lexer &l,
                  bool is_toplev)
{
    static_expr *e;

    e = static_term(l);

    while (1) {
        enum static_expr::expr_code c;

        switch (l.lastval.code) {
        case lexval::PLUS:
            c = static_expr::PLUS;
            lex(&l);
            break;

        case lexval::MINUS:
            c = static_expr::MINUS;
            lex(&l);
            break;

        case lexval::MUL:
            c = static_expr::MUL;
            lex(&l);
            break;

        case lexval::DIV:
            c = static_expr::DIV;
            lex(&l);
            break;

#define BIN(OP)                                 \
            case lexval::OP:                    \
                c = static_expr::OP;            \
            lex(&l);                            \
            break;

            BIN(LE);
            BIN(GE);
            BIN(LT);
            BIN(GT);
            BIN(NE);
            BIN(EQEQ);
#undef BIN

        default:
            return e;
        }

        static_expr *r = static_term(l);
        e = static_expr::bin(c, e, r);
    }
}

static struct static_expr *
parse_static_expr(lexer &l)
{
    return parse_static_expr(l, true);
}

program
parse(const char *source,
      int source_len)
{
    program ret;
    struct lexer l(source, source_len);
    lex(&l);

    while (1) {
        stmt s;

        switch (l.lastval.code) {
        case lexval::GOTO:
            lex(&l);
            if (l.lastval.code != lexval::SYM) {
                fprintf(stderr, "unexpected token near goto\n");
                assert(0);
            }
            s = stmt::goto_(l.lastval.u.sym);
            lex(&l);
            if (l.lastval.code != lexval::EOL) {
                fprintf(stderr, "EOL expected\n");
                assert(0);
            }
            lex(&l);
            break;

        case lexval::GOTO_IF: {
            lex(&l);
            if (l.lastval.code != lexval::SYM) {
                fprintf(stderr, "unexpected token near goto\n");
                assert(0);
            }
            char *label = l.lastval.u.sym;
            lex(&l);
            struct static_expr *se = parse_static_expr(l);
            s = stmt::goto_if(label, se);
            if (l.lastval.code != lexval::EOL) {
                fprintf(stderr, "EOL expected\n");
                assert(0);
            }
            lex(&l);
        }
            break;

        case lexval::LABEL:
            lex(&l);
            if (l.lastval.code != lexval::SYM) {
                fprintf(stderr, "unexpected token near label\n");
                assert(0);
            }
            s = stmt::label(l.lastval.u.sym);
            lex(&l);
            if (l.lastval.code != lexval::EOL) {
                fprintf(stderr, "EOL expected\n");
                assert(0);
            }
            lex(&l);
            break;

        case lexval::SET: {
            lex(&l);
            if (l.lastval.code != lexval::SYM) {
                fprintf(stderr, "unexpected token near set\n");
                assert(0);
            }
            char *symval = l.lastval.u.sym;
            lex(&l);
            if (l.lastval.code != lexval::EQ) {
                fprintf(stderr, "unexpected token expected '='\n");
                assert(0);
            }
            lex(&l);
            struct static_expr *se = parse_static_expr(l);
            s = stmt::set_var(symval, se);
            if (l.lastval.code != lexval::EOL) {
                fprintf(stderr, "EOL expected\n");
                assert(0);
            }
            lex(&l);
        }
            break;

        case lexval::EOL:
            lex(&l);
            continue;

        case lexval::EOF_:
            goto quit;

        default:
            s = stmt::expr(parse_expr(l));
            lex(&l);
            break;
        }

        ret.stmts.push_back(s);
    }
quit:
    return ret;
}

static int
lookup_label(program &prog,
             const char *src)
{
    for (unsigned int i=0; i<prog.stmts.size(); i++) {
        stmt *s = &prog.stmts[i];
        if ((s->code == stmt::LABEL) &&
            strcmp(s->u.label, src) == 0)
        {
            return i;
        }
    }

    fprintf(stderr, "undefined label %s\n", src);
    assert(0);
}

static int
eval_static_expr(program &prog,
                 static_expr *e)
{
    switch (e->code) {

#define BIN(code, op)                                   \
        case static_expr::code:                         \
        return eval_static_expr(prog, e->u.bin.l) op    \
        eval_static_expr(prog, e->u.bin.r);

        BIN(PLUS, +);
        BIN(MUL, *);
        BIN(DIV, /);
        BIN(MINUS, -);
        BIN(GE, >=);
        BIN(GT, >);
        BIN(LE, <=);
        BIN(LT, <);
        BIN(NE, !=);
        BIN(EQEQ, ==);

    case static_expr::CONST_INT:
        return e->u.val;

    case static_expr::VARREF:
        return prog.vars[e->u.var_name];
    }

    assert(0);
}

void
run(program &prog)
{
    unsigned int pc = 0;
    stmt *stmts = &prog.stmts[0];
    compiler_state st;

    while (1) {
        if (pc >= prog.stmts.size()) {
            fprintf(stderr, "program reachs to eof!!\n");
            return;
        }

        switch (stmts[pc].code) {
        case stmt::EXPR: {
            expr *e = stmts[pc].u.e;
            //fprintf(stderr, "orig = ");
            //dump_expr(e);
            //fprintf(stderr, "\n");
            expr *iexpanded = expand_integer(prog, e);
            // fprintf(stderr, "expand integer = ");
            // dump_expr(iexpanded);
            // fprintf(stderr, "\n");
            expr *sk = iexpanded;
            bool change = false;
            sk = expand_sk(sk, NULL, &change);
            // fprintf(stderr, "expand sk = ");
            // dump_expr(sk);
            // fprintf(stderr, "\n");

            struct commands commands;
            compile(st,commands, sk);
            //dump_commands(commands);

            for (unsigned int i=0; i<commands.commands.size(); i++) {
                write_line(commands.commands[i]);
                get_command_line(from_opponent);
            }
        }
            break;

        case stmt::SET_VAR: {
            int value = eval_static_expr(prog, stmts[pc].u.set_var.expr);
            prog.vars[stmts[pc].u.set_var.var_name] = value;
        }
            break;

        case stmt::LABEL:
            break;

        case stmt::GOTO:
            pc = lookup_label(prog, stmts[pc].u.label);
            continue;

        case stmt::GOTO_IF:
            if (eval_static_expr(prog, stmts[pc].u.goto_if.expr)) {
                pc = lookup_label(prog, stmts[pc].u.goto_if.label);
                continue;
            }
            break;
        }

        pc++;
    }
}

void
dump_program(program &prog)
{
    for (unsigned int i=0; i<prog.stmts.size(); i++) {
        stmt &s = prog.stmts[i];

        switch (s.code) {
        case stmt::GOTO:
            fprintf(stderr, "goto %s\n", s.u.label);
            break;

        case stmt::LABEL:
            fprintf(stderr, "label %s\n", s.u.label);
            break;

        case stmt::GOTO_IF:
            fprintf(stderr, "goto_if label=%s expr=...\n", s.u.goto_if.label);
            break;

        case stmt::SET_VAR:
            fprintf(stderr, "set (%s) = ...\n", s.u.set_var.var_name);
            break;

        case stmt::EXPR:
            fprintf(stderr, "expr ");
            dump_expr(s.u.e);
            fprintf(stderr, "\n");
            break;

        }
    }
}



}
