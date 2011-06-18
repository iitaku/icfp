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

public:
	Card(const std::string method_);
	Card(const std::string method_, int ans_);
	~Card();

	bool set(Card& card);
	bool func(int& ans_);
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
