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

    pro[16].v = -1;
    pro[32].v = -1;
    pro[127].v = -1;
    pro[19].v = -1;
    pro[29].v = -1;

    from_opponent = stdin;
    to_opponent = stdout;
}

int revive_trigger(const event_list_t& event_list, expr* e, var_map_t& vm)
{
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

void update_state(commands coms)
{
    for (int i=0; i<coms.size(); i++) {
        apply_card(coms[i].card, coms[i].lr,
                coms[i].slot, true);
        dump_slots();
    }
}

int
main(int argc, char **argv)
{
    init();

    {
        CriticalHandler ch;
        var_map_t vm;

        int step = 0;
        
        commands coms;

        hooks.programs.push_back(Hook("revive ($i)", vm, CompileParam(LEFT, 0, 128, false), ch, revive_trigger));
       
        eval_at(coms, "attack (3)(4)(8192)", vm, CompileParam(LEFT, 0, 4, false));
        update_state(coms);
        
        //eval_at(coms, "attack (10)(11)(8192)", vm, CompileParam(LEFT, 0, 4, false));
        //update_state(coms);
    }
}
