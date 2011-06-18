/* -*- tab-width: 4 ; indent-tabs-mode: t -*- */

#include <iostream>
#include "solve.hpp"
#include <stdio.h>

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

bool
Card::set(Card& card, int type_)
{
	type = type_;

	if (is_number) {
		cerr << method << endl; /////
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
				cards[0].set(cards[2], type);
				if (cards[0].is_number)
					cerr << "Error: S: f: need function, not number" << endl;
				cards[1].set(cards[2], type);
				cards[0].set(cards[1], type);
				Card c(cards[0]);
				*this = c;
#if 0
				method = cards[0].method;
				vector<Card> c(cards[0].cards);
				cards = c;
				n = cards[0].n;
				is_number = cards[0].is_number;
				ans = cards[0].ans;
#endif
				int x;
				if (func(x, type)) {
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
				if (cards[2].func(x, type)) {
					int nn = x;
					if (type == 0) pro[ii].v -= nn;
					else opp[ii].v -= nn;
					if (type == 0) opp[N_SLOTS-jj-1].v -= nn * 9 / 10;
					else pro[N_SLOTS-jj-1].v -= nn * 9 / 10;

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
				if (cards[2].func(x, type)) {
					int nn = x;
					if (type == 0) pro[ii].v -= nn;
					else opp[ii].v -= nn;
					if (type == 0) pro[jj].v += nn * 11 / 10;
					else opp[jj].v += nn * 11 / 10;

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
				Card c(cards[0]);
				*this = c;
#if 0
				is_number = cards[0].is_number;
				method = cards[0].method;
				ans = cards[0].ans;
#endif
			}
			return true;
		} else if (method == "put") {
			method = "I";
			return true;
		} else if (method == "get") {
			if (card.is_number) {
				*this = (type == 0 ? pro[card.ans].root : opp[card.ans].root);
			} else {
				cards.push_back(card);
			}
			return true;
		} else if (method == "succ") {
			cards.push_back(card);
			if (cards.size() == n) {
				int x;
				if (cards[0].func(x, type)) {
					ans = x + 1;
					is_number = true;
				}
			}
			return true;
		} else if (method == "dbl") {
			cards.push_back(card);
			if (cards.size() == n) {
				int x;
				if (cards[0].func(x, type)) {
					ans = x * 2;
					is_number = true;
				}
			}
			return true;
		} else if (method == "inc") {
			cards.push_back(card);
			if (cards.size() == n) {
				int x;
				if (cards[0].func(x, type)) {
					if (x >= 0 && x < N_SLOTS) {
						if (!is_zombie_world) (type == 0 ? pro[x].v : opp[x].v)++;
						else opp[x].v--;
					} else
						cerr << "Error: inc: exceeded limit: " << x << endl;
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
				if (cards[0].func(x, type)) {
					if (x >= 0 && x < N_SLOTS) {
						if (!is_zombie_world) opp[N_SLOTS-x-1].v--;
						else {
							if (type == 0) pro[N_SLOTS-x-1].v++;
							else opp[N_SLOTS-x-1].v++;
						}
					} else
						cerr << "Error: dec: exceeded limit: " << x << endl;
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
					if ((type == 0 ? pro[card.ans].v : opp[card.ans].v) <= 0) {
						if (type == 0) pro[card.ans].v = 1;
						else opp[card.ans] = 1;
					}
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
					opp[N_SLOTS-ii-1].root.func(x, type);
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
		} else if (method == "I") {
			*this = card;
			return true;
		}
	} else {
		if (cards[n-1].set(card, type)) {
			int x;
			if (method == "get") {
				if (cards[n-1].is_number) {
					*this = (type == 0 ? pro[cards[n-1].ans].root : opp[cards[n-1].ans].root);
				}
			} else if (method == "copy") {
				if (cards[n-1].is_number) {
					*this = (type == 0 ? opp[cards[n-1].ans].root : pro[cards[n-1].ans].root);
				}
			} else if (method == "revive") {
				if (cards[n-1].is_number) {
					int ii = cards[n-1].ans;
					if (ii >= 0 && ii < N_SLOTS) {
						if (type == 0) {
							if (pro[ii].v <= 0) pro[ii].v = 1;
						} else {
							if (opp[ii].v <= 0) opp[ii].v = 1;
						}
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
					if (cards[2].func(x, type)) {
						int nn = x;
						if (!is_zombie_world) {
							if (type == 0) {
								pro[ii].v -= nn;
								opp[N_SLOTS-jj-1].v -= nn * 9 / 10;
							} else {
								opp[ii].v -= nn;
								pro[N_SLOTS-jj-1].v -= nn * 9 / 10;
							}
						} else {
							if (type == 0) {
								opp[ii].v -= nn;
								pro[N_SLOTS-jj-1].v += nn * 9 / 10;
							} else {
								pro[ii].v -= nn;
								opp[N_SLOTS-jj-1].v += nn * 9 / 10;
							}
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
					if (cards[2].func(x, type)) {
						int nn = x;
						if (!is_zombie_world) {
							if (type == 0) {
								pro[ii].v -= nn;
								pro[jj].v += nn * 11 / 10;
							} else {
								opp[ii].v -= nn;
								opp[jj].v += nn * 11 / 10;
							}
						} else {
							if (type == 0) {
								opp[ii].v -= nn;
								opp[jj].v -= nn * 11 / 10;
							} else {
								pro[ii].v -= nn;
								pro[jj].v -= nn * 11 / 10;
							}
						}

						method = "I";
						cards.clear();
						n = 1;
						is_number = false;
					}
				}
			} else if (method == "I") {
				Card c(cards[0]);
				*this = c;
				return true;
			}
			if (func(x, type)) {
				is_number = true;
				ans = x;
				if (method == "succ") ans++;
				else if (method == "dbl") ans *= 2;
			}
			return true;
		} else {
			cerr << "Error: invalid function: " << method << endl;
			return false;
		}
	}

	return false;
}

bool Card::func(int& ans_, int type_)
{
	type = type_;

	if (cards.size() != n) {
		return false;
	}

	if (n == 0 || is_number) {
		ans_ = ans;
		return true;
	} else if (n == 1) {
		bool ret = cards[0].func(ans_, type);
		return ret;
	} else if (n == 2) {
		return cards[1].func(ans_, type);
	} else if (n == 3) {
		return cards[2].func(ans_, type);
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
		if (card.set(root, type))
			root = card;
		else
			cerr << "Error: left operation" << endl;
	} else if (apply == 2) {
		if (!root.set(card, type))
			cerr << "Error: right operation" << endl;
	} else {
		cerr << "Error: invalid apply type: " << apply << endl;
	}
}

std::vector<Slot> pro;
std::vector<Slot> opp;

static void
dump_card(Card const &c)
{
	if (c.is_number) {
		fprintf(stderr, "%d", c.ans);
	} else if (c.type == 1) {
		fprintf(stderr, "(%s ", c.method.c_str());
		for (int i=0; i<c.cards.size(); i++) {
			dump_card(c.cards[i]);
		}
		fprintf(stderr, ")");
	} else {
		fprintf(stderr, "(");
		for (int i=0; i<c.cards.size(); i++) {
			dump_card(c.cards[i]);
		}
		fprintf(stderr, "%s)", c.method.c_str());
	}
}
	

void
dump_slots()
{
	for (int i=0; i<256; i++) {
		if (pro[i].root.method == "I" &&
			pro[i].v == 10000)
		{
			/* nop */
		} else {
			fprintf(stderr, "p%d: v=%d, f=", i, pro[i].v);
			dump_card(pro[i].root);
			fprintf(stderr, "\n");
		}
	}

	for (int i=0; i<256; i++) {
		if (opp[i].root.method == "I" &&
			opp[i].v == 10000)
		{
			/* nop */
		} else {
			fprintf(stderr, "p%d: v=%d, f=", i, opp[i].v);
			dump_card(opp[i].root);
			fprintf(stderr, "\n");
		}
	}
}

}
