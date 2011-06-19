#include <stdio.h>
#include <map>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "expr.hpp"
#include "eval.hpp"
#include "translators.hpp"
#include "command.hpp"
#include "program.hpp"
#include "solve.hpp"
#include "proponents.hpp"

namespace copy_kawaii {

void init()
{
    for (int i = 0; i < N_SLOTS; i++) {
        pro.push_back(Slot());
        opp.push_back(Slot());
    }

    from_opponent = stdin;
    to_opponent = stdout;
}

program
build(commands &commands,
      const char *source)
{
    program prog = parse(source, strlen(source));
    return prog;
}
}

using namespace copy_kawaii;

char source[1024*1024];

int
main(int argc, char **argv)
{
    init();

    if (argc >= 2) {
        if (argv[1][0] == '1') {
            get_command_line(from_opponent);
        }
    }

    var_map_t vm;

#ifdef ATTACK0
    test_attack0();
#elif defined ROBUST_ZOMBIE
    demo_robust_zombie();
#else
    //zombie_help();
    //demo_left();
    //demo_ref_prev_val();
    demo_vslot();
#endif
}
