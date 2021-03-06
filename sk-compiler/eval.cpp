#include "eval.hpp"
#include "expr.hpp"
#include "translators.hpp"
#include "command.hpp"
#include "hook-program.hpp"

#include <assert.h>

namespace copy_kawaii {

HookCollection hooks;

void
eval_at(commands &coms,
        expr *expr, var_map_t &vm,
        CompileParam const &cp)
{
    MemPool p;
    bool dump = false;
    struct expr *iexpanded = expand_integer(vm, expr, cp, p);

    if (dump) {
        fprintf(stderr, "expand integer = ");
        dump_expr(iexpanded);
        fprintf(stderr, "\n");
    }
    bool change = false;
    struct expr *sk = expand_sk(iexpanded, NULL, &change, cp, p);
    if (dump) {
        fprintf(stderr, "expand sk = ");
        dump_expr(sk);
        fprintf(stderr, "\n");
    }
    compile(coms, sk, cp);

    if (dump) {
        dump_commands(coms);
    }
}

/* この関数をeval_and_run_atから呼ぶとhookがもっかい呼ばれておかしなことになったりするかも */
void
eval_at(commands &coms,
        const char *prog, var_map_t &vm,
        CompileParam const &cp)
{
    MemPool p;
    //fprintf(stderr, "eval : %s\n", prog);

    bool remove_last_zero = false;
    if (prog[0] == '!') {
        remove_last_zero = true;
        prog = prog+1;
    }

    expr *e = parse_expr(prog, cp, p);
    eval_at(coms, e, vm, cp);
    if (remove_last_zero) {
        int n = coms.size()-1;
        assert(coms[n].card == CARD_ZERO &&
               coms[n].lr == RIGHT);

        coms.erase(coms.end()-1);
    }

    for (int i=0; i<coms.size(); ++i)
    {
        /* eval_and_run_atじゃないとevent_listは意味ない */
        event_list_t event_list;
        hooks.check_and_eval_at(event_list);
    }
}

static void
gather_critical_event(event_list_t &dst,
                      const event_list_t &src,
                      CriticalHandler const &cl)
{
    for (int i=0; i<src.size(); i++) {
        const Event &ev = src[i];
        switch (ev.code) {
        case Event::PROP_DEAD: {
            if(ev.u.slot < MAX_NUM_VSLOT) {
                if (ev.u.slot == 0 &&
                    cl.auto_recovery_zero)
                {
                    VSlot prog_slot = VSA->alloc_vslot("revive0");
                    var_map_t vm;
                    CriticalHandler ch;

                    //VSA->dump();

                    if (! is_id(pro[prog_slot.slot].f)) {
                        eval_and_run_at("clear",
                                        vm,
                                        CompileParam(RIGHT, prog_slot, prog_slot, false), ch);
                    }

                    commands coms;
                    coms.push_back(command(RIGHT, prog_slot.slot, CARD_ZERO));
                    coms.push_back(command(LEFT, prog_slot.slot, CARD_REVIVE));

                    for (int i=0; i<2; i++) {
                        write_line(coms[i]);
                        get_command_line(from_opponent);
                    }
                    VSA->free_vslot(prog_slot);

                    //dst.push_back(Event(Event::REVIVE_ZERO_FAILED));
                    continue;
                }

#if defined(DUEL_IN_LOCAL)
                std::string slot_name = vsa->slot_to_name[ev.u.slot];
#else
                std::string slot_name = vsa.slot_to_name[ev.u.slot];
#endif

                critical_slots_t::const_iterator i = cl.critical_slots.begin(),
                    e = cl.critical_slots.end();
                
                for (; i!=e; ++i) {
                    if (i->first == slot_name) {
                        /* critical slot is dead !! */
                        dst.push_back(ev);
                    }
                }
            }
        }
            break;

        case Event::TYPE_ERROR:
            if (cl.type_error_critical) {
                dst.push_back(ev);
            }
            break;

        case Event::OPP_PUT_ZOMBIE:
            break;
        }
    }
}

event_list_t
eval_and_run_at(expr *e,
                var_map_t &vm,
                CompileParam const &cp,
                CriticalHandler &ch)
{
    commands coms;
    eval_at(coms, e, vm, cp);

    for (int i=0; i<coms.size(); i++) {
        event_list_t write_event = write_line(coms[i]);

        command_line cl = get_command_line(from_opponent);
        event_list_t critical_event;

        gather_critical_event(critical_event, write_event, ch);
        gather_critical_event(critical_event, cl.events, ch);

        if (critical_event.size() != 0) {

            bool r = ch.recovery(critical_event);
            if (!r) {
                return critical_event;
            }
        }
    }

    return event_list_t();
}

event_list_t
eval_and_run_at(const char *prog,
                var_map_t &vm,
                CompileParam const &cp,
                CriticalHandler &ch)
{
    commands coms;
    eval_at(coms, prog, vm, cp);

    for (int i=0; i<coms.size(); i++) {
        event_list_t write_event = write_line(coms[i]);

        command_line cl = get_command_line(from_opponent);
        event_list_t critical_event;

        gather_critical_event(critical_event, write_event, ch);
        gather_critical_event(critical_event, cl.events, ch);

        /* 使用済みスロットとかマークしなくて大丈夫か？ */
        hooks.check_and_eval_and_run_at(write_event);

        if (critical_event.size() != 0) {
            bool r = ch.recovery(critical_event);
            if (!r) {
                return critical_event;
            }
        }
    }

    

    return event_list_t();
}

}
