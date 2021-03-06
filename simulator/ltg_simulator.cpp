// ICFPC 2011, 17-20 Jun. 2011
//   by N. Futatsugi

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <stack>
#include <algorithm>
#include <numeric>
#include <utility>
#include <cmath>
#include <functional>
#include <cstdlib>
#include <complex>
//#include <xmmintrin.h>
//#include <emmintrin.h>

//#pragma pack(push,16)

#include "ltg_simulator.h"

#ifdef _MSC_VER
#include <ctime>
inline double get_time()
{
	return static_cast<double>(std::clock()) / CLOCKS_PER_SEC;
}
#else
#include <sys/time.h>
inline double get_time()
{
	timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec + 1e-6 * tv.tv_usec;
}
#endif

#define MY_RAND_MAX -1UL

inline unsigned long xor128(){
	static unsigned long x=123456789, y=362436069, z=521288629, w=88675123;
	unsigned long t;
	t = x^(x<<11);  x = y;  y = z;  z = w;
	return w = (w^(w>>19))^(t^(t>>8));
}

using namespace std;

#ifdef LOCAL
const double TIME_LIMIT = 30.0;
#else
const double TIME_LIMIT = 58.0;
#endif

static bool is_zombie_world = false;

const int N_SLOTS = 256;

// proponent and opponents' slots
static vector<Slot> pro;
static vector<Slot> opp;

Card::Card(const string method_)
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

Card::Card(const string method_, int ans_)
{
	method = method_;
	if (method != "number") {
		cerr << "Error: method should be number, not others: " << method << endl;
		return;
	}
	n = 0;
	is_number = true;
	ans = ans_;
}

Card::~Card()
{
}

bool Card::set(Card& card, int type_)
{
	type = type_;

	if (is_number) {
		cerr << "Error: number is not function: " << method << endl; ///
		return false;
	}

	if (cards.size() < n) {
		// set argument in function

		if (method == "S") {
			////////////////////////////////////////////////////////////////////
			// S f g x
			//   h <- f x
			//   y <- g x
			//   z <- h y
			//   return z
			if ((cards.size() == 0 || cards.size() == 1) && card.is_number) {
				cerr << "Error: S: fisrt / second arg in S should be function, not number" << endl;
				return false;
			}
			cards.push_back(card);
			if (cards.size() == n) {
				// h <- f x
				cards[0].set(cards[2], type);
				if (cards[0].is_number)
					cerr << "Error: S: f: need function, not number" << endl;
				// y <- g x
				cards[1].set(cards[2], type);
				// z <- h y
				cards[0].set(cards[1], type);

				// return z
				Card c(cards[0]);
				*this = c;

				int x;
				if (func(x, type)) {
					is_number = true;
					ans = x;
				}
			}
			return true;

		} else if (method == "attack") {
			////////////////////////////////////////////////////////////////////
			// attack i j n
			//   v[i] <- v[i] - n
			//   v[255-j] <- v[255-j] - n * 9 / 10
			//   return I
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
					// v[i] <- v[i] - n
					if (type == 0) {
						pro[ii].v -= nn;
						if (pro[ii].v < 0) pro[ii].v = 0;
					} else {
						opp[ii].v -= nn;
						if (opp[ii].v < 0) opp[ii].v = 0;
					}
					// v[255-j] <- v[255-j] - n * 9 / 10
					if (type == 0) {
						opp[N_SLOTS-jj-1].v -= nn * 9 / 10;
						if (opp[N_SLOTS-jj-1].v < 0) opp[N_SLOTS-jj-1].v = 0;
					} else {
						pro[N_SLOTS-jj-1].v -= nn * 9 / 10;
						if (pro[N_SLOTS-jj-1].v < 0) pro[N_SLOTS-jj-1].v = 0;
					}

					// return I
					method = "I";
					cards.clear();
					n = 1;
					is_number = false;
				}
			}
			return true;

		} else if (method == "help") {
			////////////////////////////////////////////////////////////////////
			// help i j n
			//   v[i] <- v[i] - n
			//   v[255-j] <- v[255-j] + n * 11 / 10
			//   return I
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
					// v[i] <- v[i] - n
					if (type == 0) {
						pro[ii].v -= nn;
						if (pro[ii].v < 0) pro[ii].v = 0;
					} else {
						opp[ii].v -= nn;
						if (opp[ii].v < 0) opp[ii].v = 0;
					}
					// v[255-j] <- v[255-j] + n * 11 / 10
					if (type == 0) {
						pro[jj].v += nn * 11 / 10;
						if (pro[jj].v > 65535) pro[jj].v = 65535;
					} else {
						opp[jj].v += nn * 11 / 10;
						if (opp[jj].v > 65535) opp[jj].v = 65535;
					}

					// return I
					method = "I";
					cards.clear();
					n = 1;
					is_number = false;
				}
			}
			return true;

		} else if (method == "K") {
			////////////////////////////////////////////////////////////////////
			// K x y
			//   return x
			cards.push_back(card);
			if (cards.size() == n) {
				// return x
				Card c(cards[0]);
				*this = c;
			}
			return true;

		} else if (method == "put") {
			////////////////////////////////////////////////////////////////////
			// put x
			//   return I
			method = "I";
			return true;

		} else if (method == "get") {
			////////////////////////////////////////////////////////////////////
			// get i
			//   y <- f[i]
			//   return y
			if (card.is_number) {
				// y <- f[i]
				// return y
				*this = (type == 0 ? pro[card.ans].root : opp[card.ans].root);
			} else {
				cards.push_back(card);
			}
			return true;

		} else if (method == "succ") {
			////////////////////////////////////////////////////////////////////
			// succ n
			//   m <- n + 1
			//   return m
			cards.push_back(card);
			if (cards.size() == n) {
				int x;
				if (cards[0].func(x, type)) {
					// m <- n + 1
					// return m
					ans = x + 1;
					if (ans > 65535) ans = 65535;
					is_number = true;
				}
			}
			return true;

		} else if (method == "dbl") {
			////////////////////////////////////////////////////////////////////
			// dbl n
			//   m <- n * 2
			//   return m
			cards.push_back(card);
			if (cards.size() == n) {
				int x;
				if (cards[0].func(x, type)) {
					// m <- n * 2
					// return m
					ans = x * 2;
					if (ans > 65535) ans = 65535;
					is_number = true;
				}
			}
			return true;

		} else if (method == "inc") {
			cards.push_back(card);
			////////////////////////////////////////////////////////////////////
			// inc i
			//   v[i] <- v[i] + 1
			//   return I
			if (cards.size() == n) {
				int x;
				if (cards[0].func(x, type)) {
					if (x >= 0 && x < N_SLOTS) {
						// v[i] <- v[i] + 1
						if (!is_zombie_world) {
							if (type == 0 && pro[x].v > 0 && pro[x].v < 65535) pro[x].v++;
							else if (type == 1 && opp[x].v > 0 && opp[x].v < 65535) opp[x].v++;
						} else {
							if (type == 0 && opp[x].v > 0 && opp[x].v < 65535) opp[x].v--;
							else if (type == 1 && pro[x].v > 0 && pro[x].v < 65535) pro[x].v--;
						}
					} else
						cerr << "Error: inc: exceeded limit: " << x << endl;
				}
			}

			// return I
			method = "I";
			cards.clear();
			is_number = false;
			n = 1;
			return true;

		} else if (method == "dec") {
			////////////////////////////////////////////////////////////////////
			// dec i
			//   v[255-i] <- v[255-i] - 1
			//   return I
			cards.push_back(card);
			if (cards.size() == n) {
				int x;
				if (cards[0].func(x, type)) {
					if (x >= 0 && x < N_SLOTS) {
						// v[255-i] <- v[255-i] - 1
						if (!is_zombie_world) {
							if (type == 0 && opp[N_SLOTS-x-1].v > 0) opp[N_SLOTS-x-1].v--;
							else if (type == 1 && pro[N_SLOTS-x-1].v > 0) pro[N_SLOTS-x-1].v--;
						} else {
							if (type == 0 && pro[N_SLOTS-x-1].v > 0) pro[N_SLOTS-x-1].v++;
							else if (type == 1 && opp[N_SLOTS-x-1].v > 0) opp[N_SLOTS-x-1].v++;
						}
					} else
						cerr << "Error: dec: exceeded limit: " << x << endl;
				}
			}

			// return I
			method = "I";
			cards.clear();
			is_number = false;
			n = 1;
			return true;

		} else if (method == "copy") {
			////////////////////////////////////////////////////////////////////
			// copy i
			//   y <- f'[i]
			//   return y

			// y <- f'[i]
			// return y
			if (card.is_number) *this = (type == 0 ? opp[card.ans].root : pro[card.ans].root);
			else cards.push_back(card);
			return true;

		} else if (method == "revive") {
			////////////////////////////////////////////////////////////////////
			// revice i
			//   v[i] <- 1
			//   return I
			cards.push_back(card);
			if (card.is_number) {
				if (card.ans >= 0 && card.ans < N_SLOTS) {
					if ((type == 0 ? pro[card.ans].v : opp[card.ans].v) <= 0) {
						// v[i] <- 1
						if (type == 0) pro[card.ans].v = 1;
						else opp[card.ans] = 1;
					}
				} else {
					cerr << "Error: copy: invalid slot number: " << card.ans << endl; ///
				}

				// return I
				method = "I";
				cards.clear();
				n = 1;
				is_number = false;
			}
			return true;

		} else if (method == "zombie") {
			////////////////////////////////////////////////////////////////////
			// zombie i x
			//   f'[255-i] <- x
			//   v'[255-i] < -1
			//   return I
			if (cards.size() == 0 && !card.is_number) {
				cerr << "Error: zombie: fisrt arg should be number, not function" << endl;
				return false;
			}
			cards.push_back(card);
			if (cards.size() == n) {
				int ii = cards[0].ans;
				if (ii >= 0 && ii < N_SLOTS && (type == 0 ? opp[N_SLOTS-ii-1].v : pro[N_SLOTS-ii-1].v)<= 0) {
					if (type == 0) opp[N_SLOTS-ii-1].root = cards[1];
					else pro[N_SLOTS-ii-1].root = cards[1];
					int x;
					// f'[255-i] <- x
					is_zombie_world = true;
					if (type == 0) opp[N_SLOTS-ii-1].root.func(x, type);
					else pro[N_SLOTS-ii-1].root.func(x, type);
					// v'[255-i] < -1
					is_zombie_world = false;
					if (type == 0) opp[N_SLOTS-ii-1].v = -1;
					else pro[N_SLOTS-ii-1].v = -1;
				} else {
					cerr << "Error: zombie: invalid values: " << ii << ", " << (type == 0 ? opp[N_SLOTS-ii-1].v : pro[N_SLOTS-ii-1].v) << endl; ///
				}

				// return I
				method = "I";
				cards.clear();
				n = 1;
				is_number = false;
			}
			return true;

		} else if (method == "I") {
			////////////////////////////////////////////////////////////////////
			// I x
			//   return x

			// return x
			*this = card;
			return true;
		}

	} else {
		// put function in last argument

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
								if (pro[ii].v < 0) pro[ii].v = 0;
								opp[N_SLOTS-jj-1].v -= nn * 9 / 10;
								if (opp[N_SLOTS-jj-1].v < 0) opp[N_SLOTS-jj-1].v = 0;
							} else {
								opp[ii].v -= nn;
								if (opp[ii].v < 0) opp[ii].v = 0;
								pro[N_SLOTS-jj-1].v -= nn * 9 / 10;
								if (pro[N_SLOTS-jj-1].v < 0) pro[N_SLOTS-jj-1].v = 0;
							}
						} else {
							if (type == 0) {
								opp[ii].v -= nn;
								if (opp[ii].v < 0) opp[ii].v = 0;
								pro[N_SLOTS-jj-1].v += nn * 9 / 10;
								if (pro[N_SLOTS-jj-1].v > 65535) pro[N_SLOTS-jj-1].v = 65535;
							} else {
								pro[ii].v -= nn;
								if (pro[ii].v < 0) pro[ii].v = 0;
								opp[N_SLOTS-jj-1].v += nn * 9 / 10;
								if (opp[N_SLOTS-jj-1].v > 65535) opp[N_SLOTS-jj-1].v = 65535;
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
								if (pro[ii].v < 0) pro[ii].v = 0;
								pro[jj].v += nn * 11 / 10;
								if (pro[jj].v > 65535) pro[jj].v = 65535;
							} else {
								opp[ii].v -= nn;
								if (opp[ii].v < 0) opp[ii].v = 0;
								opp[jj].v += nn * 11 / 10;
								if (opp[jj].v > 65535) opp[jj].v = 65535;
							}
						} else {
							if (type == 0) {
								opp[ii].v -= nn;
								if (opp[ii].v < 0) opp[ii].v = 0;
								opp[jj].v -= nn * 11 / 10;
								if (opp[jj].v < 0) opp[jj].v = 0;
							} else {
								pro[ii].v -= nn;
								if (pro[ii].v < 0) pro[ii].v = 0;
								pro[jj].v -= nn * 11 / 10;
								if (pro[jj].v < 0) pro[jj].v = 0;
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
				if (ans > 65535) ans = 65535;
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

Slot::Slot(int type_) : type(type_), cnt(1), v(10000), root(Card("I"))
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

int main()
{
	for (int i = 0; i < N_SLOTS; i++) {
		pro.push_back(Slot());
		opp.push_back(Slot(1));
	}

#if 0
	pro[0].set(Card("get"), 1);
	pro[0].set(Card("put"), 1);

	pro[0].set(Card("S"), 1);
	pro[0].set(Card("succ"), 2);
	pro[0].set(Card("succ"), 2);
	pro[0].set(Card("zero"), 2);
	pro[0].set(Card("succ"), 1);

	//pro[0].set(Card("zero"), 2);
	//pro[0].set(Card("get"), 1);
	//pro[0].set(Card("zero"), 2);
	pro[0].set(Card("succ"), 1);
	pro[0].set(Card("dbl"), 1);
	pro[0].set(Card("I"), 1);
	pro[0].set(Card("I"), 1);
	pro[0].set(Card("I"), 1);
	pro[0].set(Card("I"), 1);
	pro[0].set(Card("I"), 1);

#if 0
	pro[0].set(Card("K"), 1);
	pro[0].set(Card("zero"), 2);
	//pro[0].set(Card("inc"), 1);
	pro[0].set(Card("dec"), 1);
	pro[0].set(Card("succ"), 1);
	pro[0].set(Card("zero"), 2);
#endif  

	//pro[0].set(Card("attack"), 1);
	pro[0].set(Card("help"), 1);
	pro[0].set(Card("number", 10), 2);
	pro[0].set(Card("number", 100), 2);
	pro[0].set(Card("zero"), 2);
	pro[0].set(Card("copy"), 1);
	pro[0].set(Card("succ"), 2);
	pro[0].set(Card("zero"), 2);

	pro[1].v = -1;
	opp[N_SLOTS-1-1].v = 0;
	pro[0].set(Card("revive"), 1);
	pro[0].set(Card("succ"), 2);
	pro[0].set(Card("zero"), 2);
	pro[0].set(Card("zombie"), 1);
	pro[0].set(Card("zero"), 2);
	pro[0].set(Card("zero"), 2);
#endif

#if 0  // test for get
	pro[0].set(Card("get"), 2);
	pro[0].set(Card("dbl"), 2);
	pro[0].set(Card("succ"), 2);
	pro[0].set(Card("zero"), 2);
	pro[0].set(Card("succ"), 2);
	pro[0].set(Card("zero"), 2);
#endif

#if 0  // test for zombie
	pro[0].set(Card("zero"), 2);
	pro[0].set(Card("zombie"), 1);
	pro[0].set(Card("S"), 1);
	pro[0].set(Card("succ"), 2);
	pro[0].set(Card("zero"), 2);
#endif

#if 0  // test for SKKI
	pro[0].set(Card("S"), 2);
	pro[0].set(Card("K"), 2);
	pro[0].set(Card("K"), 2);
	pro[0].set(Card("I"), 2);
	pro[0].set(Card("zero"), 2);
	pro[0].set(Card("succ"), 1);
#endif

#if 1  // test for zombie-KS attack
	opp[255].v = 0;
	pro[0].set(Card("zombie"), 2);
	pro[0].set(Card("zero"), 2);
	pro[0].set(Card("K"), 1);
	pro[0].set(Card("S"), 1);
	pro[0].set(Card("succ"), 2);
	pro[0].set(Card("K"), 1);
	pro[0].set(Card("S"), 1);
	pro[0].set(Card("succ"), 2);
	pro[0].set(Card("K"), 1);
	pro[0].set(Card("S"), 1);
	pro[0].set(Card("succ"), 2);
	pro[0].set(Card("zero"), 2);
	pro[0].set(Card("zero"), 2);
#endif

#if 0  // test for debug
	pro[0].set(Card("put"), 1);
	pro[0].set(Card("zero"), 2);
	pro[0].set(Card("succ"), 1);
	pro[0].set(Card("dbl"), 1);
#endif

	int ans = 0;
	cerr << "---" << endl; ////
	cerr << pro[0].root.method << endl; /////
	cerr << boolalpha << pro[0].root.func(ans) << endl; /////
	//cerr << boolalpha << pro[0].root.is_number << endl; /////
	cerr << ans << endl; /////
	cerr << pro[10].v << endl; /////
	cerr << opp[N_SLOTS-10-1].v << endl; /////

	cerr << pro[1].v << endl; /////

#if 1  // test for opponent
	opp[0].set(Card("S"), 2);
	opp[0].set(Card("K"), 2);
	opp[0].set(Card("K"), 2);
	opp[0].set(Card("I"), 2);
	opp[0].set(Card("succ"), 1);
	opp[0].set(Card("zero"), 2);
#endif

	cerr << "---" << endl; ////
	cerr << opp[0].root.method << endl; /////
	cerr << boolalpha << opp[0].root.func(ans) << endl; /////
	cerr << ans << endl; /////
	cerr << opp[10].v << endl; /////
	cerr << pro[N_SLOTS-10-1].v << endl; /////
	cerr << opp[1].v << endl; /////

	return 0;
}
