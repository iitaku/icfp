/* -*- tab-width: 4 ; indent-tabs-mode: t -*- */

#ifndef SOLVE_HPP
#define SOLVE_HPP

#include <vector>
#include <string>

#include "tool.hpp"
#include "events.hpp"

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
			int cur_applied;
			Card *args[3];
		} func;
	} u;

public:

	/**
	 * @brief constructor
	 * @param[in] method_  card type
	 * @param[in] ans_     set number to card (number card only)
	 */
	Card(enum card_code func);
	Card(int ans_) {
		is_number = true;
		ans = ans_;
		n = 0;
	}
	~Card();

	/**
	 * @brief application applies (as a function) a card
	 * @param[in] card_    card
	 * @param[in] type_    proponent (0) or opponent (1)
	 */
	bool set(Card& card, int type_=0);

	/**
	 * @brief execute function
	 * @param[out] ans_    return value from function
	 * @param[in]  type_   proponent (0) or opponent (1)
	 */
	bool func(int& ans_, int type_=0);
};

/* return nothing() if type error is occured */
optional<Card *>apply(event_list_t &events,
					  Card *func,
					  Card *arg,
					  int slot,
					  bool is_pro);

/**
 * @brief Slot class
 */
struct Slot {
	// vitality
	int v;
	// field value (not implement yet; use slots[*].root.ans or slots[*].func(ans))
	Card *f

	/**
	 * @brief constructor
	 * @param[in] type_    proponent (0) or opponent (1)
	 */
	Slot();
	~Slot();

	/**
	 * @brief set a card on a slot
	 * @param[in] card_    card
	 * @param[in] apply_   left application (1) or right application (2)
	 */
	event_list_t set(Card card, int apply);
};

// proponent and opponents' slots
extern std::vector<Slot> pro;
extern std::vector<Slot> opp;

void dump_slots();

}

#endif // SOLVE_HPP
