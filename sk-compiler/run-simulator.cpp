#include "solve.hpp"
#include "command.hpp"
#include "eval.hpp"

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

int
main(int argc, char **argv)
{
    init();

    {
        CriticalHandler ch;
        var_map_t vm;

        commands coms;

        eval_at(coms, "attack (3)(4)", vm, CompileParam(LEFT, 0, 4, false));

        for (int i=0; i<coms.size(); i++) {
            apply_card(coms[i].card, coms[i].lr,
                       coms[i].slot, true);
            dump_slots();
        }
    }
                    

}
