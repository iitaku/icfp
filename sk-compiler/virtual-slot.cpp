#include <stdio.h>
#include <limits.h>
#include "solve.hpp"
#include "virtual-slot.hpp"

namespace copy_kawaii {

static int
genint_cost(int val)
{
    int i = 0;
    int max_bitpos = 0;
    int bit_count = 0;
    /* 線形探索ひどい */
    for (i=0; i<24; i++) {
        if (val & (1<<i)) {
            max_bitpos = i;
            bit_count++;
        }
    }

    /* n回シフト + nビット分加算 */
    return max_bitpos + bit_count;
}

#if defined(DUEL_IN_LOCAL)
VirtualSlotAllocator vsa_[2];
__thread VirtualSlotAllocator* vsa;
#else
VirtualSlotAllocator vsa;
#endif


VSlot
VirtualSlotAllocator::alloc_vslot(const char *name,
                                  enum alloc_hint hint)
{
    int min = MAX_NUM_VSLOT;
    int min_cost = INT_MAX;
    for (int i=0; i<MAX_NUM_VSLOT; i++) {
        int i_cost = genint_cost(i);

        if (states[i] == FREE && pro[i].v > 0) {
            if (i_cost < min_cost) {
                min = i;
                min_cost = i_cost;
            }
        }
    }

    if (min == MAX_NUM_VSLOT) {
        /* oh.. */
        fprintf(stderr, "warn : reserve vslot failed\n");        
        return VSlot(false, 0, name);
    }

    states[min] = USED;
    name_to_slot[name] = min;
    slot_to_name[min] = name;

    return VSlot(true, min, name);
}

VSlot
VirtualSlotAllocator::reserve(const char *name, int slot)
{
    if (states[slot] != FREE) {
        fprintf(stderr, "warn : reserve vslot failed\n");
        return alloc_vslot(name);
    }

    states[slot] = USED;
    name_to_slot[name] = slot;
    slot_to_name[slot] = name;

    return VSlot(true, slot, name);
}

void
VirtualSlotAllocator::free_vslot(VSlot const &vs)
{
    if (states[vs.slot] != DEAD) {
        states[vs.slot] = FREE;
    }

    slot_to_name[vs.slot] = "<unassigned>";
    name_to_slot[vs.name] = -1;
}

void
VirtualSlotAllocator::dump()
{
    for (int i=0; i<MAX_NUM_VSLOT; i++) {
        switch (states[i]) {
        case FREE:
            //fprintf(stderr, "%4d:free\n", i);
            break;

        case DEAD:
            fprintf(stderr, "%4d:dead\n", i);
            break;

        case USED:
            fprintf(stderr, "%4d: %s\n", i, slot_to_name[i].c_str());
            break;
        }
    }
}

}
