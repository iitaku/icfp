#ifndef STATS_HPP
#define STATS_HPP

#include <vector>
#include <algorithm>
#include <utility>

namespace copy_kawaii {

typedef enum Player { PRO=0, OPP } player_t;

struct SlotInfo {
	int slot;
	int used;
	int field;
	int attacked;
	int damage;
	int heal;
	int v;

	SlotInfo(int slot_) : slot(slot_), used(0), attacked(0), damage(0), heal(0), v(0) { }
};

class FindSlot {
private:
	int slot;

public:
	FindSlot(int slot_) : slot(slot_) { }
	bool operator()(const SlotInfo& a) {
		return slot == a.slot;
	}
};

enum SlotDataType { USED=0, ATTACKED, DAMAGE, HEAL };

template<SlotDataType T> class SortSlots {
public:
	bool operator()(const SlotInfo& a, const SlotInfo& b) {
		switch (T) {
		case USED: return a.used > b.used;
		case ATTACKED: return a.attacked > b.attacked;
		case DAMAGE: return a.damage > b.damage;
		case HEAL: return a.heal > b.heal;
		}
	}
};

struct Stats {
	struct Player {
		std::vector<SlotInfo> slots;
	} pro, opp;

	template<SlotDataType T>
	void count(player_t p, int slot_) {
		Player& player = (p == PRO) ? pro : opp;
		std::vector<SlotInfo>::iterator it = find_if(player.slots.begin(), player.slots.end(), FindSlot(slot_));
		if (it == player.slots.end()) {
			player.slots.push_back(SlotInfo(slot_));
			it = player.slots.end() - 1;
		}
		
		int *data;
		switch (T) {
		case USED: data = &it->used;  break;
		case ATTACKED: data = &it->attacked;  break;
		}
		(*data)++;
	}

	template<SlotDataType T>
	void add(player_t p, int slot_, int value_) {
		Player& player = (p == PRO) ? pro : opp;
		std::vector<SlotInfo>::iterator it = find_if(player.slots.begin(), player.slots.end(), FindSlot(slot_));
		if (it == player.slots.end()) {
			player.slots.push_back(SlotInfo(slot_));
			it = player.slots.end() - 1;
		}
		
		int *data;
		switch (T) {
		case DAMAGE: data = &it->damage;  break;
		case HEAL: data = &it->heal;  break;
		}
		(*data) += value_;
	}

	void proc() {
		std::sort(pro.slots.begin(), pro.slots.end(), SortSlots<USED>());
		std::sort(opp.slots.begin(), opp.slots.end(), SortSlots<USED>());
	}
};

}

#endif // STATS_HPP
