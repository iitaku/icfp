/* -*- tab-width: 4 ; indent-tabs-mode: t -*- */

#ifndef SOLVE_HPP
#define SOLVE_HPP

#include <vector>
#include <string>

namespace copy_kawaii {

const int N_SLOTS = 256;

class Card {
public:
	std::string method;
	std::vector<Card> cards;
	int ans;
	int n;
	bool is_number;
	int type;

public:
	Card(const std::string method_);
	Card(const std::string method_, int ans_);
	Card(const Card& card) {
		method = card.method;
		cards = card.cards;
		ans = card.ans;
		n = card.n;
		is_number = card.is_number;
	}
	~Card();

	bool set(Card& card, int type_=0);
	bool func(int& ans_, int type_=0);
};

class Slot {
private:
	int type; // true: proponent, false: opponent

public:
	Card root;
	int cnt;
	int v, f;

	Slot(int type_=0);
	~Slot();

	void set(Card card, int apply);
};

extern std::vector<Slot> pro;
extern std::vector<Slot> opp;

}

#endif
