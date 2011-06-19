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

class SortUsed {
public:
	bool operator()(const SlotInfo& a, const SlotInfo& b)
		{ return a.used > b.used; }
};

class SortAttacked {
public:
	bool operator()(const SlotInfo& a, const SlotInfo& b)
		{ return a.attacked > b.attacked; }
};

struct Stats {
	struct Player {
		std::vector<SlotInfo> slots;
	} pro, opp;

	Stats() { }
	void count_used(player_t p, int slot_) {
		Player& player = (p == PRO) ? pro : opp;
		std::vector<SlotInfo>::iterator it = find_if(player.slots.begin(), player.slots.end(), FindSlot(slot_));
		if (it != player.slots.end()) it->used++;
		else {
			player.slots.push_back(SlotInfo(slot_));
			player.slots.back().used++;
		}
	}
	void count_attacked(player_t p, int slot_) {
		Player& player = (p == PRO) ? pro : opp;
		std::vector<SlotInfo>::iterator it = find_if(player.slots.begin(), player.slots.end(), FindSlot(slot_));
		if (it != player.slots.end()) it->attacked++;
		else {
			player.slots.push_back(SlotInfo(slot_));
			player.slots.back().attacked++;
		}
	}
	void damage(player_t p, int slot_, int damage_) {
		Player& player = (p == PRO) ? pro : opp;
		std::vector<SlotInfo>::iterator it = find_if(player.slots.begin(), player.slots.end(), FindSlot(slot_));
		if (it != player.slots.end()) it->damage += damage_;
		else {
			player.slots.push_back(SlotInfo(slot_));
			player.slots.back().damage += damage_;
		}
	}
	void heal(player_t p, int slot_, int heal_) {
		Player& player = (p == PRO) ? pro : opp;
		std::vector<SlotInfo>::iterator it = find_if(player.slots.begin(), player.slots.end(), FindSlot(slot_));
		if (it != player.slots.end()) it->heal += heal_;
		else {
			player.slots.push_back(SlotInfo(slot_));
			player.slots.back().heal += heal_;
		}
	}
	void proc() {
#if 0
		std::sort(pro.slots.begin(), pro.slots.end(), SortUsed());
		std::sort(opp.slots.begin(), opp.slots.end(), SortUsed());
#endif
		std::sort(pro.slots.begin(), pro.slots.end(), SortAttacked());
		std::sort(opp.slots.begin(), opp.slots.end(), SortAttacked());
	}
};

}

#endif // STATS_HPP
