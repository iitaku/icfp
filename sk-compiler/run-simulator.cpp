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

        apply_card(CARD_I, RIGHT, 0, false);
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

        //hooks.programs.push_back(Hook("revive ($i)", vm, CompileParam(LEFT, 0, 128, false), ch, revive_trigger));

        eval_at(coms, "0", vm, CompileParam(LEFT, 0, 12, false));
        update_state(coms);

        eval_at(coms, "10000", vm, CompileParam(LEFT, 0, 8, false));
        update_state(coms);

        eval_at(coms, "attack (3)(4)(6000)", vm, CompileParam(LEFT, 3, 4, false));
        update_state(coms);
        eval_at(coms, "attack (4)(5)(5000)", vm, CompileParam(LEFT, 3, 4, false));
        update_state(coms);
        eval_at(coms, "(S (K (help (0) (1))) (K (@8)))",
                vm, CompileParam(RIGHT,0,3,false));
        update_state(coms);
        eval_at(coms, "zombie (4) @3",
                vm, CompileParam(RIGHT,0,4,false));
        update_state(coms);
        
        //eval_at(coms, "attack (10)(11)(8192)", vm, CompileParam(LEFT, 0, 4, false));
        //update_state(coms);
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
