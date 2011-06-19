#ifndef EVENT_HPP
#define EVENT_HPP

#include <vector>

namespace copy_kawaii {

struct Event {
    enum event_code {
        REVIVE_ZERO_FAILED,
        PROP_DEAD,
        TYPE_ERROR,
        OPP_PUT_ZOMBIE
    } code;

    union {
        int slot;

        struct {
            
        } xyzzy;

        float xyzzy2;
    } u;

    Event(event_code e)
        :code(e)
    {}

    Event() {}

    static Event dead(enum event_code code, int slot) {
        Event e;
        //e.code = PROP_DEAD;
		e.code = code;
        e.u.slot = slot;
        return e;
    }

    static Event type_error(int slot) {
        Event e;
        e.code = TYPE_ERROR;
        e.u.slot = slot;
        return e;
    }
};

typedef std::vector<Event> event_list_t;

}

#endif
