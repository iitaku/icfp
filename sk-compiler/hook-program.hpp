#ifndef HOOK_PROGRAM_HPP
#define HOOK_PROGRAM_HPP

#include <vector>

#include "eval.hpp"

namespace copy_kawaii { 

/* 
 * hookの発動条件記述関数 
 */
typedef int (*TriggerFunc_t)(var_map_t& vm);

struct Hook
{
    const char* prog_;
    var_map_t vm_;
    CompileParam cp_;
    CriticalHandler *ch_;
    commands* coms_;
    TriggerFunc_t trigger_;

    Hook() {}
    
    Hook(const char* prog,
         var_map_t vm,
         CompileParam cp,
         CriticalHandler *ch,
         TriggerFunc_t trigger)
        : prog_(prog), vm_(vm), cp_(cp), ch_(ch), coms_(NULL), trigger_(trigger)
    {}

    Hook(const char* prog,
         var_map_t vm,
         CompileParam cp,
         CriticalHandler *ch,
         commands *coms,
         TriggerFunc_t trigger)
        : prog_(prog), vm_(vm), cp_(cp), ch_(ch), coms_(coms), trigger_(trigger)
    {}

};

struct HookCollection 
{
    /* hookの再帰の深さを */
    //int recursive_cnt = 0;
    //int recursive_max = 1;
    bool hooked;
    std::vector<Hook> programs;

    void check_and_eval_at(void)
    {
        if (false == hooked)
        {
            hooked = true;
            for (int i=0; i<programs.size(); ++i)
            {
                if (programs[i].trigger_(programs[i].vm_))
                {
                    eval_at(*programs[i].coms_,
                            programs[i].prog_,
                            programs[i].vm_,
                            programs[i].cp_);
                }
            }
            hooked = false;
        }
    }

    void check_and_eval_and_run_at(void)
    {
        if (false == hooked)
        {
            hooked = true;
            for (int i=0; i<programs.size(); ++i)
            {
                if (programs[i].trigger_(programs[i].vm_))
                {
                    eval_and_run_at(programs[i].prog_,
                                    programs[i].vm_,
                                    programs[i].cp_,
                                    *programs[i].ch_);
                }
            }
            hooked = false;
        }
    }
    
    HookCollection() : hooked(false), programs(0) {}
};

extern HookCollection hooks;

}

#endif
