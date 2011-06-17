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
        SYM,
        END
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
            if (isalpha(c)) {
                symbuf[0] = c;
                sympos = 1;
                while (1) {
                    c = lgetc(l);
                    if (! isalpha(c)) {
                        lungetc(l, c);

                        symbuf[sympos] = '\0';
                        name_to_card_t::iterator i = name_to_card.find(symbuf);
                        if (i == name_to_card.end()) {
                            if (strcmp("goto", symbuf) == 0) {
                                return l->lastval = lexval(lexval::GOTO);
                            }
                            if (strcmp("label", symbuf) == 0) {
                                return l->lastval = lexval(lexval::LABEL);
                            }
                            if (strcmp("end", symbuf) == 0) {
                                return l->lastval = lexval(lexval::END);
                            }

                            return l->lastval = lexval::symval(strdup(symbuf));

                            fprintf(stderr, "unknown card name '%s'\n", symbuf);
                            abort();
                        }

                        return l->lastval = lexval(i->second);
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
        }
    }
}


static struct expr *
parse_expr(lexer &l)
{
    return parse_expr(&l, true);
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

        case lexval::EOL:
            lex(&l);
            break;

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
    int i;
    for (int i=0; i<prog.stmts.size(); i++) {
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


void
run(program &prog)
{
    int pc = 0;
    stmt *stmts = &prog.stmts[0];
    compiler_state st;

    while (1) {
        switch (stmts[pc].code) {
        case stmt::EXPR: {
            expr *e = stmts[pc].u.e;
            //fprintf(stderr, "orig = ");
            //dump_expr(e);
            //fprintf(stderr, "\n");
            expr *iexpanded = expand_integer(e);
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

        case stmt::LABEL:
            
            break;

        case stmt::GOTO:
            pc = lookup_label(prog, stmts[pc].u.label);
            continue;
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

        case stmt::EXPR:
            fprintf(stderr, "expr ");
            dump_expr(s.u.e);
            fprintf(stderr, "\n");
            break;

        }
    }
}



}
