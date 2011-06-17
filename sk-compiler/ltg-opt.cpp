#include <stdio.h>
#include <map>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "expr.hpp"
#include "translators.hpp"
#include "command.hpp"

namespace copy_kawaii {
name_to_card_t name_to_card;

const char *card_to_name_table[] = {
    "I", "zero", "succ", "dbl", "get", "put",
    "S", "K", "inc", "dec", "attack",
    "help", "copy", "revive", "zombie"
};

inline const char *
get_card_name(enum card_code cc)
{
    return card_to_name_table[(int)cc];
}

struct lexval {
    enum lcode {
        EOL,
        LPAR,
        RPAR,
        CARD,
        DIGIT
    } code;

    union {
        enum card_code card;
        int digit_val;
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
            return l->lastval = lexval(lexval::EOL);
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

void
dump_expr(expr *e)
{
    switch (e->code) {
    case expr::APPLY:
        fprintf(stderr, "(");
        dump_expr(e->u.apply.f);
        dump_expr(e->u.apply.a);
        fprintf(stderr, ") ");
        break;

    case expr::CARD:
        fprintf(stderr, "%s ", get_card_name(e->u.card));
        break;

    case expr::INTEGER:
        fprintf(stderr, "%d ", e->u.int_val);
        break;

    case expr::EMIT_INC_COUNTER:
        fprintf(stderr, "<emit_inc %d> ", e->u.int_val);
        break;

    }
}

static struct expr *
parse(const char *line,
      int linelen)
{
    struct lexer l(line, linelen);
    lex(&l);
    return parse_expr(&l, true);
}

void init()
{
    from_opponent = stdin;
    to_opponent = stdout;

    name_to_card["I"] = CARD_I;
    name_to_card["zero"] = CARD_ZERO;
    name_to_card["succ"] = CARD_SUCC;
    name_to_card["dbl"] = CARD_DBL;
    name_to_card["get"] = CARD_GET;
    name_to_card["put"] = CARD_PUT;
    name_to_card["S"] = CARD_S;
    name_to_card["K"] = CARD_K;
    name_to_card["inc"] = CARD_INC;
    name_to_card["dec"] = CARD_DEC;
    name_to_card["attack"] = CARD_ATTACK;
    name_to_card["help"] = CARD_HELP;
    name_to_card["copy"] = CARD_COPY;
    name_to_card["revive"] = CARD_REVIVE;
    name_to_card["zombie"] = CARD_ZOMBIE;
}

void
build(commands &commands,
      const char *prog)
{
    expr *e = parse(prog, strlen(prog));
    //fprintf(stderr, "orig = ");
    //dump_expr(e);
    //fprintf(stderr, "\n");

    expr *iexpanded =  expand_integer(commands, e);
    fprintf(stderr, "expand integer = ");
    dump_expr(iexpanded);
    fprintf(stderr, "\n");

    expr *sk = iexpanded;

    bool change = false;
    sk = expand_sk(commands, sk, NULL, &change);
    fprintf(stderr, "expand sk = ");
    dump_expr(sk);
    fprintf(stderr, "\n");

    compile(commands, sk);
    dump_commands(commands);
}
}

using namespace copy_kawaii;


const char *progs[] = {
    "attack (0)(1)(15)"
};


int
main(int argc, char **argv)
{
    init();

    //fprintf(stderr, "%s\n", argv[1]);
    commands coms;

    if (argc < 2) {
        build(coms, progs[0]);
        return 0;
    }
    if (argv[1][0] == '1') {
        get_command_line(from_opponent);
    }

    build(coms, progs[0]);

    for (unsigned int i=0; i<coms.commands.size(); i++) {
        write_line(coms.commands[i]);
        get_command_line(from_opponent);        
    }
}

