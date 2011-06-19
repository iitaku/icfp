#ifndef HOOK_PROGRAM_HPP
#define HOOK_PROGRAM_HPP

#include <vector>

#include "eval.hpp"
#include "solve.hpp"

namespace copy_kawaii { 

//enum HookReturnCode
//{
//    
//}

/* 
 * hookの発動条件記述関数 
 */
typedef int (*TriggerFunc_t)(const event_list_t& event_list, expr* e, var_map_t& vm);

struct Hook
{
    const char* prog_;
    var_map_t vm_;
    CompileParam cp_;
    CriticalHandler ch_;
    TriggerFunc_t trigger_;

    Hook() {}
    
    Hook(const char* prog,
         var_map_t vm,
         CompileParam cp,
         CriticalHandler ch,
         TriggerFunc_t trigger)
        : prog_(prog), vm_(vm), cp_(cp), ch_(ch), trigger_(trigger)
    {}
};

struct HookCollection 
{
    /* hookの再帰の深さを */
    //int recursive_cnt = 0;
    //int recursive_max = 1;
    bool hooked;
    std::vector<Hook> programs;

    void check_and_eval_at(const event_list_t& event_list)
    {
        if (false == this->hooked)
        {
            this->hooked = true;
            for (int i=0; i<programs.size(); ++i)
            {
                MemPool p;
                expr *e = parse_expr(programs[i].prog_, programs[i].cp_, p);

                if (programs[i].trigger_(event_list, e, programs[i].vm_))
                {
                    commands coms;
                    eval_at(coms, e, 
                            programs[i].vm_, 
                            programs[i].cp_);
                    
                    for (int i=0; i<coms.size(); i++) {
                        apply_card(coms[i].card, coms[i].lr,
                                coms[i].slot, true);
                        dump_slots();
                    }
                }
            }
            this->hooked = false;
        }
    }

    void check_and_eval_and_run_at(const event_list_t& event_list)
    {
        if (false == hooked)
        {
            hooked = true;
            for (int i=0; i<programs.size(); ++i)
            {
                MemPool p;
                expr *e = parse_expr(programs[i].prog_, programs[i].cp_, p);
                
                if (programs[i].trigger_(event_list, e, programs[i].vm_))
                {
                    eval_and_run_at(e,
                                    programs[i].vm_,
                                    programs[i].cp_,
                                    programs[i].ch_);
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
