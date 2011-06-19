#include "solve.hpp"
#include "command.hpp"
#include "eval.hpp"
#include "virtual-slot.hpp"
#include "hook-program.hpp"

using namespace copy_kawaii;

void init()
{
    for (int i = 0; i < N_SLOTS; i++) {
        pro.push_back(Slot());
        opp.push_back(Slot());
    }

    from_opponent = stdin;
    to_opponent = stdout;
}

int revive_trigger(const event_list_t& event_list, expr* e, var_map_t& vm)
{
    dump_expr(e);
    for (int i=0; i<pro.size(); ++i)
    {
        if (pro[i].v < 0)
        {
            vm["i"] = i;
            return 1;
        }
    }

    return 0;
}

void update_state(commands &coms)
{
    for (int i=0; i<coms.size(); i++) {
        event_list_t el = apply_card(coms[i].card, coms[i].lr,
                                     coms[i].slot, true);
        for (int i=0; i<el.size(); i++) {
            switch (el[i].code) {
            case Event::PROP_DEAD:
                fprintf(stderr, "%d dead\n",
                        el[i].u.slot);
                break;
            }
        }
        dump_slots();

        apply_card(coms[i].card, coms[i].lr, coms[i].slot, false);
    }
    coms.clear();
}

void sim(void)
{
    init();

    {
        CriticalHandler ch;
        var_map_t vm;

        int step = 0;
        commands coms;

        eval_at(coms, "!@1 @1", vm, CompileParam(RIGHT, 0, 1, false));

        update_state(coms);
    }

    dump_slots();
}

int
main(int argc, char **argv)
{
    sim_log = stderr;
    sim();
    return 0;
}
