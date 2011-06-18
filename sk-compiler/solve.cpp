/* -*- tab-width: 4 ; indent-tabs-mode: t -*- */

#include <iostream>
#include "solve.hpp"

namespace copy_kawaii {

using namespace std;

static bool is_zombie_world = false;

Card::Card(const std::string method_)
{
	method = method_;
	is_number = false;
	if (method == "zero" || method == "number") {
		n = 0;
		ans = 0;
		is_number = true;
	} else if (method == "S" || method == "attack" || method == "help") {
		n = 3;
	} else if (method == "K" || method == "zombie") {
		n = 2;
	} else {
		n = 1;
	}
}

Card::Card(const std::string method_, int ans_)
{
	method = method_;
	if (method != "number") {
		std::cerr << "Error: method should be number, not others: " << method << std::endl;
		return;
	}
	n = 0;
	is_number = true;
	ans = ans_;
}

Card::~Card()
{
}

bool Card::set(Card& card)
{
	if (is_number) {
		cerr << "Error: number is not function: " << method << endl; ///
		return false;
	}

	if (cards.size() < n) {
		if (method == "S") {
			if ((cards.size() == 0 || cards.size() == 1) && card.is_number) {
				cerr << "Error: S: fisrt / second arg in S should be function, not number" << endl;
				return false;
			}
			cards.push_back(card);
			if (cards.size() == n) {
				cards[0].set(cards[2]);
				if (cards[0].is_number)
					cerr << "Error: S: f: need function, not number" << endl;
				cards[1].set(cards[2]);
				cards[0].set(cards[1]);
				*this = cards[0];
				int x;
				if (func(x)) {
					is_number = true;
					ans = x;
				}
				return true;
			}
			return true;
		} else if (method == "attack") {
			if ((cards.size() == 0 || cards.size() == 1) && !card.is_number) {
				cerr << "Error: attack: fisrt / second arg in S should be number, not function" << endl;
				return false;
			}
			cards.push_back(card);
			if (cards.size() == n) {
				int ii = cards[0].ans;
				int jj = cards[1].ans;
				int x;
				if (cards[2].func(x)) {
					int nn = x;
					pro[ii].v -= nn;
					opp[N_SLOTS-jj-1].v -= nn * 9 / 10;

					method = "I";
					cards.clear();
					n = 1;
					is_number = false;
				}
				return true;
			}
			return true;
		} else if (method == "help") {
			if ((cards.size() == 0 || cards.size() == 1) && !card.is_number) {
				cerr << "Error: help: fisrt / second arg in S should be number, not function" << endl;
				return false;
			}
			cards.push_back(card);
			if (cards.size() == n) {
				int ii = cards[0].ans;
				int jj = cards[1].ans;
				int x;
				if (cards[2].func(x)) {
					int nn = x;
					pro[ii].v -= nn;
					pro[jj].v += nn * 11 / 10;

					method = "I";
					cards.clear();
					n = 1;
					is_number = false;
				}
				return true;
			}
			return true;
		} else if (method == "K") {
			cards.push_back(card);
			if (cards.size() == n) {
				//*this = cards[0];
				is_number = cards[0].is_number;
				method = cards[0].method;
				ans = cards[0].ans;
			}
			return true;
		} else if (method == "put") {
			method = "I";
			return true;
		} else if (method == "get") {
			if (card.is_number) {
				*this = pro[card.ans].root;
			} else {
				cards.push_back(card);
			}
			return true;
		} else if (method == "succ") {
			cards.push_back(card);
			if (cards.size() == n) {
				int x;
				if (cards[0].func(x)) {
					ans = x + 1;
					is_number = true;
				}
			}
			return true;
		} else if (method == "dbl") {
			cards.push_back(card);
			if (cards.size() == n) {
				int x;
				if (cards[0].func(x)) {
					ans = x * 2;
					is_number = true;
				}
			}
			return true;
		} else if (method == "inc") {
			cards.push_back(card);
			if (cards.size() == n) {
				int x;
				if (cards[0].func(x)) {
					if (x >= 0 && x < N_SLOTS) {
						if (!is_zombie_world) pro[x].v++;
						else opp[x].v--;
					} else
						cerr << "inc: exceeded limit: " << x << endl;
				}
			}
			method = "I";
			cards.clear();
			is_number = false;
			n = 1;
			return true;
		} else if (method == "dec") {
			cards.push_back(card);
			if (cards.size() == n) {
				int x;
				if (cards[0].func(x)) {
					if (x >= 0 && x < N_SLOTS) {
						if (!is_zombie_world) opp[N_SLOTS-x-1].v--;
						else pro[N_SLOTS-x-1].v++;
					} else
						cerr << "dec: exceeded limit: " << x << endl;
				}
			}
			method = "I";
			cards.clear();
			is_number = false;
			n = 1;
			return true;
		} else if (method == "copy") {
			if (card.is_number) {
				*this = opp[card.ans].root;
			} else {
				cards.push_back(card);
			}
			return true;
		} else if (method == "revive") {
			cards.push_back(card);
			if (card.is_number) {
				if (card.ans >= 0 && card.ans < N_SLOTS) {
					if (pro[card.ans].v <= 0)
						pro[card.ans].v = 1;
				} else {
					cerr << "Error: copy: invalid slot number: " << card.ans << endl; ///
				}
				method = "I";
				cards.clear();
				n = 1;
				is_number = false;
			}
			return true;
		} else if (method == "zombie") {
			if (cards.size() == 0 && !card.is_number) {
				cerr << "Error: zombie: fisrt arg should be number, not function" << endl;
				return false;
			}
			cards.push_back(card);
			if (cards.size() == n) {
				int ii = cards[0].ans;
				if (ii >= 0 && ii < N_SLOTS && opp[N_SLOTS-ii-1].v <= 0) {
					opp[N_SLOTS-ii-1].root = cards[1];
					int x;
					is_zombie_world = true;
					opp[N_SLOTS-ii-1].root.func(x);
					is_zombie_world = false;
					opp[N_SLOTS-ii-1].v = -1;
				} else {
					cerr << "Error: zombie: invalid values: " << ii << ", " << opp[N_SLOTS-ii-1].v << endl; ///
				}
				method = "I";
				cards.clear();
				n = 1;
				is_number = false;
			}
			return true;
		}
#if 1
		cards.push_back(card);
		if (cards.size() == n) {
			int x;
			if (func(x)) {
				is_number = true;
				ans = x;
			}
		}
		return true;
#endif
	} else {
		if (cards[n-1].set(card)) {
			int x;
			if (method == "get") {
				if (cards[n-1].is_number) {
					*this = pro[cards[n-1].ans].root;
				}
			} else if (method == "copy") {
				if (cards[n-1].is_number) {
					*this = opp[cards[n-1].ans].root;
				}
			} else if (method == "revive") {
				if (cards[n-1].is_number) {
					int ii = cards[n-1].ans;
					if (ii >= 0 && ii < N_SLOTS) {
						if (pro[ii].v <= 0)
							pro[ii].v = 1;
					} else {
						cerr << "Error: copy: invalid slot number: " << card.ans << endl; ///
					}
					method = "I";
					cards.clear();
					n = 1;
					is_number = false;
				}
			} else if (method == "attack") {
				if (cards[n-1].is_number) {
					int ii = cards[0].ans;
					int jj = cards[1].ans;
					int x;
					if (cards[2].func(x)) {
						int nn = x;
						if (!is_zombie_world) {
							pro[ii].v -= nn;
							opp[N_SLOTS-jj-1].v -= nn * 9 / 10;
						} else {
							opp[ii].v -= nn;
							pro[N_SLOTS-jj-1].v += nn * 9 / 10;
						}

						method = "I";
						cards.clear();
						n = 1;
						is_number = false;
					}
				}
			} else if (method == "help") {
				if (cards[n-1].is_number) {
					int ii = cards[0].ans;
					int jj = cards[1].ans;
					int x;
					if (cards[2].func(x)) {
						int nn = x;
						if (!is_zombie_world) {
							pro[ii].v -= nn;
							pro[jj].v += nn * 11 / 10;
						} else {
							opp[ii].v -= nn;
							opp[jj].v -= nn * 11 / 10;
						}

						method = "I";
						cards.clear();
						n = 1;
						is_number = false;
					}
				}
			}
			if (func(x)) {
				is_number = true;
				ans = x;
				if (method == "succ") ans++;
				else if (method == "dbl") ans *= 2;
			}
			return true;
		} else {
			cerr << "Error: invalid function:" << method << endl;
			return false;
		}
	}

	return false;
}

bool Card::func(int& ans_)
{
	if (cards.size() != n) return false;

	if (n == 0 || is_number) {
		ans_ = ans;
		return true;
	} else if (n == 1) {
		return cards[0].func(ans_);
	} else if (n == 2) {
		return cards[1].func(ans_);
	} else if (n == 3) {
		return cards[2].func(ans_);
	}
	return false;
}

Slot::Slot(int type_)
	: type(type_), root(Card("I")), cnt(1), v(10000)
{
}

Slot::~Slot()
{
}

void Slot::set(Card card, int apply)
{
	if (apply == 1) {
		if (card.set(root))
			root = card;
		else
			cerr << "Error: left operation" << endl;
	} else if (apply == 2) {
		if (!root.set(card))
			cerr << "Error: right operation" << endl;
	} else {
		cerr << "Error: invalid apply type: " << apply << endl;
	}
}

std::vector<Slot> pro;
std::vector<Slot> opp;

}
