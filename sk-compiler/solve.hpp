/* -*- tab-width: 4 ; indent-tabs-mode: t -*- */

#ifndef SOLVE_HPP
#define SOLVE_HPP

#include <vector>
#include <string>

#include "tool.hpp"
#include "events.hpp"
#include "optional.hpp"
#include "command.hpp"

namespace copy_kawaii {

const int N_SLOTS = 256;

/**
 * @brief Card class
 */
class Card {
public:
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
	Slot()
		: v(10000), f(new Card(CARD_I))
	{}

	~Slot()
	{}

	/**
	 * @brief set a card on a slot
	 * @param[in] card_    card
	 * @param[in] apply_   left application (1) or right application (2)
	 */
};

event_list_t apply_card(enum card_code card, enum lr_code lr, int slot, bool is_pro);


// proponent and opponents' slots
extern std::vector<Slot> pro;
extern std::vector<Slot> opp;

void dump_slots();

}

#endif // SOLVE_HPP
