/* -*- tab-width: 4 ; indent-tabs-mode: t -*- */

#ifndef SOLVE_HPP
#define SOLVE_HPP

#include <vector>
#include <string>

#include "tool.hpp"
#include "events.hpp"
#include "optional.hpp"
#include "command.hpp"
#include "stats.hpp"

namespace copy_kawaii {

const int N_SLOTS = 256;

static Stats stats;

/**
 * @brief Card class
 */
class Card {
public:
	Card *chain;

	bool mark;
	bool is_number;
	// card type : 15 types
	enum card_code card;
	// arguments : 0-3

	union {
		int int_val;
		struct {
			int cur_num_applied;
			Card *args[3];
		} func;
	} u;

public:
	Card(enum card_code method_)
	{
		card = method_;
		is_number = false;
		u.func.cur_num_applied = 0;
	}

	Card(int v) {
		is_number = true;
		u.int_val = v;
	}
};

static inline
bool is_id(Card const *c) {
	if (c->is_number) {
		return false;
	}

	if (c->card != CARD_I) {
		return false;
	}
	return true;
}

/* return nothing() if type error is occured */
optional<Card *>apply(event_list_t &events,
					  Card *func,
					  Card *arg,
					  int slot,
					  bool is_pro,
					  bool is_zombie_apply);

/**
 * @brief Slot class
 */
struct Slot {
	// vitality
	int v;
	// field value (not implement yet; use slots[*].root.ans or slots[*].func(ans))
	Card *f;

	/**
	 * @brief constructor
	 * @param[in] type_    proponent (0) or opponent (1)
	 */
	Slot();

	~Slot()
	{}

	/**
	 * @brief set a card on a slot
	 * @param[in] card_    card
	 * @param[in] apply_   left application (1) or right application (2)
	 */
};

event_list_t apply_card(enum card_code card, enum lr_code lr, int slot, bool is_pro);
void gc_card();

// proponent and opponents' slots
extern std::vector<Slot> pro;
extern std::vector<Slot> opp;
extern FILE *sim_log;

void dump_slots();

}

#endif // SOLVE_HPP
