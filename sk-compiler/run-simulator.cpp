#include "solve.hpp"
#include "command.hpp"
#include "eval.hpp"
#include "hook-program.hpp"

using namespace copy_kawaii;

void init()
{
    for (int i = 0; i < N_SLOTS; i++) {
        pro.push_back(Slot());
        opp.push_back(Slot());
    }

    pro[0].v = -1;

    from_opponent = stdin;
    to_opponent = stdout;
}

int revive_trigger(var_map_t& vm)
{
    for (int i=0; i<pro.size(); ++i)
    {
        if (0 < pro[i].v)
        {
            vm["$i"] = i;
            return 1;
        }
    }

    return 0;
}

int
main(int argc, char **argv)
{
    init();

    {
        CriticalHandler ch;
        var_map_t vm;

        commands coms;

        hooks.programs.push_back(Hook("revive ($i)", vm, CompileParam(LEFT, 0, 128, false), &ch, &coms, revive_trigger));
        
        //eval_at(coms, "attack (3)(4)(8192)", vm, CompileParam(LEFT, 0, 4, false));
        eval_at(coms, "help  (3)(4)(8192)", vm, CompileParam(LEFT, 0, 4, false));

        for (int i=0; i<coms.size(); i++) {
            apply_card(coms[i].card, coms[i].lr,
                       coms[i].slot, true);
            dump_slots();
        }
    }
}
