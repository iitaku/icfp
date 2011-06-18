// ICFPC 2011, 17-20 Jun. 2011
//   by N. Futatsugi

#include <vector>
#include <string>

/**
 * @brief Card class
 */
class Card {
public:
	// card type : 15 types
	std::string method;
	// arguments : 0-3
	std::vector<Card> cards;
	// return value
	int ans;
	// number of arguments
	int n;
	// this card is number ?
	bool is_number;
	// proponent (0) or opponent (1)
	int type;

public:
    /**
	 * @brief constructor
	 * @param[in] method_  card type
	 * @param[in] ans_     set number to card (number card only)
	 */
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

/**
 * @brief Slot class
 */
class Slot {
private:
	// proponent (0) or opponent (1)
	int type;

public:
	// root of cards (functions)
	Card root;
	// count cards (not implement yet)
	int cnt;
	// vitality
	int v;
	// field value (not implement yet; use slots[*].root.ans or slots[*].func(ans))
	int f;

    /**
	 * @brief constructor
	 * @param[in] type_    proponent (0) or opponent (1)
	 */
	Slot(int type_=0);
	~Slot();

    /**
	 * @brief set a card on a slot
	 * @param[in] card_    card
	 * @param[in] apply_   left application (1) or right application (2)
	 */
	void set(Card card, int apply);
};
