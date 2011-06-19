#ifndef COPY_KAWAII_VIRTUAL_SLOT_HPP
#define COPY_KAWAII_VIRTUAL_SLOT_HPP

#include <string>
#include <map>

namespace copy_kawaii {

struct VSlot {
    bool live;
    int slot;
    std::string name;

    VSlot(bool live,
          int slot,
          const std::string name)
        :live(live),
         slot(slot),
         name(name)
    {}
};

#define MAX_NUM_VSLOT 128

typedef std::map<std::string, int> name_to_slot_t;

struct VirtualSlotAllocator {
    enum state_code {
        FREE,
        USED,
        DEAD
    };

    /* 今は使ってないです */
    enum alloc_hint {
        PRI_SMALL_INDEX,
        PRI_VITAL
    };

    enum state_code states[MAX_NUM_VSLOT];
    std::string slot_to_name[MAX_NUM_VSLOT];
    name_to_slot_t name_to_slot;

    VSlot reserve(const char *name, int slot);
    VSlot alloc_vslot(const char *name, enum alloc_hint hint = PRI_SMALL_INDEX);

    void free_vslot(VSlot const &vs);

    VirtualSlotAllocator() {
        for (int i=0; i<MAX_NUM_VSLOT; i++) {
            states[i] = FREE;
        }
    }

    void dump();
};

extern VirtualSlotAllocator vsa;

}

#endif
