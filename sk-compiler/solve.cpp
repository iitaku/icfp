/* -*- tab-width: 4 ; indent-tabs-mode: t -*- */

#include <iostream>
#include <cstdio>
#include "solve.hpp"

namespace copy_kawaii {

using namespace std;

static const int num_arg[] = {
    1,                          // I
    0,                          // ZERO
    1,                          // SUCC
    1,                          // DBL
    1,                          // GET
    1,                          // PUT
    3,                          // S
    2,                          // K
    1,                          // INC
    1,                          // DEC
    3,                          // ATTACK
    3,                          // HELP
    1,                          // COPY
    1,                          // REVIBE
    2,                          // ZOMBIE,
    0                           // ?
}


static bool is_zombie_world = false;

event_list_t Slot::event;

Card::Card(enum card_code method_)
{
	method = method_;
	is_number = false;

	switch (method) {
	case CARD_ZERO:
		is_number = true;
		n = 0;
		ans = 0;
		break;
		
	case CARD_S:
	case CARD_ATTACK:
	case CARD_HELP:
		n = 3;
		break;

	case CARD_K:
	case CARD_ZOMBIE:
		n = 2;
		break;

	default:
		n = 1;
		break;
	}
}

Card::~Card()
{
}

optional<Card *>
apply(event_list_t &events,
	  Card *func,
	  Card *arg,
	  int slot,
	  bool is_pro)
{
	if (func->is_number == 0) {
		if (is_pro) {
			events.push_back(Event::type_error(slot));
		}

		return nothing();
	}

	int full_num_arg = num_arg[func->card];
	int remain = full_num_arg - func->u.func->cur_applied;;

	assert(remain != 0);

	if (remain == 1) {
		/* all arguments are applied. fold it */
		Card *h,*y,*z,*f,*g,*n, *i, *j;

		switch (func->card) {
		case CARD_S:
			////////////////////////////////////////////////////////////////////
			// S f g x
			//   h <- f x
			//   y <- g x
			//   z <- h y
			//   return z
			f = func->u.func.args[0];
			g = func->u.func.args[1];
			x = func->u.func.args[2];

			h = apply(events, f, x, slot, is_pro);
			if (!h) {
				return nothing();
			}

			y = apply(events, g, x, slot, is_pro);
			if (!y) {
				return nothing();
			}

			z = apply(events, *h, *y, slot, is_pro);
			return z;

		case CARD_ATTACK: {
			////////////////////////////////////////////////////////////////////
			// attack i j n
			//   v[i] <- v[i] - n
			//   v[255-j] <- v[255-j] - n * 9 / 10
			//   return I

			i = func->u.func.args[0];
			j = func->u.func.args[1];
			n = func->u.func.args[2];

			if ((! i->is_number) || (! j->is_number) || (! n->is_number)) {
				// cerr << "Error: attack: fisrt / second arg in S should be number, not function" << endl;
				return nothing();
			}
			int ii = i->u.int_val;
			int jj = i->u.int_val;
			int nn = n->u.int_val;

			if (ii < 0 || ii >= N_SLOTS || jj < 0 || jj >= N_SLOTS) {
				// cerr << "Error: attack: 0 <= index <= 255: " << ii << ", " << jj << endl;
				return nothing();
			}

			if (is_pro) {
				pro[ii].v -= nn;
				if (pro[ii].v <= 0) {
					pro[ii].v = 0;
					events.push_back(Event::dead(Event::PROP_DEAD, ii));
				}
				opp[255-jj-1].v -= nn * 9 / 10;
				if (opp[255-jj].v < 0) opp[255-jj].v = 0;
			} else {
				opp[ii].v -= nn;
				if (opp[ii].v < 0) opp[ii].v = 0;
				pro[255-jj].v -= nn * 9 / 10;
				if (pro[255-jj].v <= 0) {
					pro[255-jj].v = 0;
					Slot::event.push_back(Event::dead(Event::PROP_DEAD, 255-jj));
				}
			}
		}
			return new Card(CARD_I);

		case CARD_HELP: {
			////////////////////////////////////////////////////////////////////
			// help i j n
			//   v[i] <- v[i] - n
			//   v[255-j] <- v[255-j] + n * 11 / 10
			//   return I

			i = func->u.func.args[0];
			j = func->u.func.args[1];
			n = func->u.func.args[2];

			if ((! i->is_number) || (! j->is_number) || (! n->is_number)) {
				// cerr << "Error: attack: fisrt / second arg in S should be number, not function" << endl;
				return nothing();
			}
			int ii = i->u.int_val;
			int jj = i->u.int_val;
			int nn = n->u.int_val;

			if (ii < 0 || ii >= N_SLOTS || jj < 0 || jj >= N_SLOTS) {
				// cerr << "Error: attack: 0 <= index <= 255: " << ii << ", " << jj << endl;
				return nothing();
			}

			if (is_pro) {
				pro[ii].v -= nn;
				if (pro[ii].v <= 0) {
					pro[ii].v = 0;
					events.push_back(Event::dead(Event::PROP_DEAD, ii));
				}
				pro[jj].v += nn * 11 / 10;
				if (pro[jj].v > 65535) pro[jj].v = 65535;
			} else {
				opp[ii].v -= nn;
				if (opp[ii].v < 0) opp[ii].v = 0;

				opp[jj].v += nn * 11 / 10;
				if (opp[jj].v > 65535) opp[jj].v = 65535;
			}
		}
			return new Card(CARD_I);

		case CARD_K: {
			////////////////////////////////////////////////////////////////////
			// K x y
			//   return x
			x = func->u.args[0];
			// y = func->u.args[1];

			return x;
		}

		case CARD_PUT:
			return new Card(CARD_I);

		case CARD_GET: {
			////////////////////////////////////////////////////////////////////
			// get i
			//   y <- f[i]
			//   return y

			i = func->u.args[0];
			if (! i->is_number) {
				return nothing();
			}

			if (is_pro) {
				y = pro[i->u.int_val].f;
			} else {
				y = opp[i->u.int_val].f;
			}

			y = card_deep_copy(y);

			return y;
		}

		case CARD_SUCC: {
			////////////////////////////////////////////////////////////////////
			// succ n
			//   m <- n + 1
			//   return m

			n = func->u.args[0];
			if (! n->is_number) {
				return nothing();
			}

			m = new Card(n->u.int_val + 1);
			return m;
		}

		case CARD_DBL: {
			////////////////////////////////////////////////////////////////////
			// dbl n
			//   m <- n * 2
			//   return m
			
			n = func->u.args[0];
			if (! n->is_number) {
				return nothing();
			}

			m = new Card(n->u.int_val * 2);
			return m;
		}

		case CARD_INC: {
			////////////////////////////////////////////////////////////////////
			// inc i
			//   v[i] <- v[i] + 1
			//   return I

			i = func->u.args[0];

			if (! i->is_number) {
				return nothing();
			}

			int idx = i->u.int_val;

			if (is_pro) {
				if (is_zombie_apply) {
					if ((pro[idx].v > 0) && (pro[idx].v < 65535)) {
						pro[idx].v--;
						if (pro[idx].v <= 0) {
							Slot::event.push_back(Event::dead(Event::PROP_DEAD, idx));
						}
					}
				} else {
					if ((pro[idx].v > 0) && (pro[idx].v < 65535)) {
						pro[idx].v++;
					}
				}
			} else {
				if (is_zombie_apply) {
					if ((opp[idx].v > 0) && (opp[idx].v < 65535)) {
						opp[idx].v--;
					}
				} else {
					if ((opp[idx].v > 0) && (opp[idx].v < 65535)) {
						opp[idx].v++;
					}
				}
			}
			return new Card(CARD_I);
		}

		case CARD_DEC:
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
							if (pro[N_SLOTS-x-1].v <= 0)
								Slot::event.push_back(Event::dead(Event::PROP_DEAD, N_SLOTS-x-1));
						} else {
							if (type == 0 && pro[N_SLOTS-x-1].v > 0) pro[N_SLOTS-x-1].v++;
							else if (type == 1 && opp[N_SLOTS-x-1].v > 0) opp[N_SLOTS-x-1].v++;
						}
					} else
						cerr << "Error: dec: exceeded limit: " << x << endl;
				}
			}

			// return I
			method = CARD_I;
			cards.clear();
			is_number = false;
			n = 1;
			return true;

		} else if (method == CARD_COPY) {
			////////////////////////////////////////////////////////////////////
			// copy i
			//   y <- f'[i]
			//   return y

			// y <- f'[i]
			// return y
			if (card.is_number) {
				*this = (type == 0 ? opp[card.ans].root : pro[card.ans].root);
				//Card c(type == 0 ? opp[card.ans].root : pro[card.ans].root);
				//*this = c;
			} else cards.push_back(card);
			return true;

		} else if (method == CARD_REVIVE) {
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
				method = CARD_I;
				cards.clear();
				n = 1;
				is_number = false;
			}
			return true;

		} else if (method == CARD_ZOMBIE) {
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
				method = CARD_I;
				cards.clear();
				n = 1;
				is_number = false;
			}
			return true;

		} else if (method == CARD_I) {
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
			if (method == CARD_GET) {
				if (cards[n-1].is_number) {
					*this = (type == 0 ? pro[cards[n-1].ans].root : opp[cards[n-1].ans].root);
					//Card c(type == 0 ? pro[cards[n-1].ans].root : opp[cards[n-1].ans].root);
					//*this = c;
				}
			} else if (method == CARD_COPY) {
				if (cards[n-1].is_number) {
					*this = (type == 0 ? opp[cards[n-1].ans].root : pro[cards[n-1].ans].root);
					//Card c(type == 0 ? opp[cards[n-1].ans].root : pro[cards[n-1].ans].root);
					//*this = c;
				}
			} else if (method == CARD_REVIVE) {
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
					method = CARD_I;
					cards.clear();
					n = 1;
					is_number = false;
				}
			} else if (method == CARD_ATTACK) {
				if (cards[n-1].is_number) {
					int ii = cards[0].ans;
					int jj = cards[1].ans;
					int x;
					if (cards[2].func(x, type)) {
						int nn = x;
						if (!is_zombie_world) {
							if (type == 0) {
								pro[ii].v -= nn;
								if (pro[ii].v <= 0) {
									pro[ii].v = 0;
									Slot::event.push_back(Event::dead(Event::PROP_DEAD, ii));
								}
								opp[N_SLOTS-jj-1].v -= nn * 9 / 10;
								if (opp[N_SLOTS-jj-1].v < 0) opp[N_SLOTS-jj-1].v = 0;
							} else {
								opp[ii].v -= nn;
								if (opp[ii].v < 0) opp[ii].v = 0;
								pro[N_SLOTS-jj-1].v -= nn * 9 / 10;
								if (pro[N_SLOTS-jj-1].v <= 0) {
									pro[N_SLOTS-jj-1].v = 0;
									Slot::event.push_back(Event::dead(Event::PROP_DEAD, N_SLOTS-jj-1));
								}
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

						method = CARD_I;
						cards.clear();
						n = 1;
						is_number = false;
					}
				}
			} else if (method == CARD_HELP) {
				if (cards[n-1].is_number) {
					int ii = cards[0].ans;
					int jj = cards[1].ans;
					int x;
					if (cards[2].func(x, type)) {
						if (ii < 0 || ii >= N_SLOTS || jj < 0 || ii >= N_SLOTS) {
							cerr << "Error: help: 0 <= index <= 255: " << ii << ", " << jj << endl;
							return false;
						}
						int nn = x;
						if (!is_zombie_world) {
							if (type == 0) {
								pro[ii].v -= nn;
								if (pro[ii].v <= 0) {
									pro[ii].v = 0;
									Slot::event.push_back(Event::dead(Event::PROP_DEAD, ii));
								}
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
								if (pro[ii].v <= 0) {
									pro[ii].v = 0;
									Slot::event.push_back(Event::dead(Event::PROP_DEAD, ii));
								}
								pro[jj].v -= nn * 11 / 10;
								if (pro[jj].v <= 0) {
									pro[jj].v = 0;
									Slot::event.push_back(Event::dead(Event::PROP_DEAD, jj));
								}
							}
						}

						method = CARD_I;
						cards.clear();
						n = 1;
						is_number = false;
					}
				}
			} else if (method == CARD_I) {
				Card c(cards[0]);
				*this = c;
				return true;
			}
			if (func(x, type)) {
				is_number = true;
				ans = x;
				if (method == CARD_SUCC) ans++;
				else if (method == CARD_SUCC) ans *= 2;
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

Slot::Slot(int type_) : type(type_), cnt(1), v(10000), root(Card(CARD_I))
{
}

Slot::~Slot()
{
}

event_list_t Slot::set(Card card, int apply)
{
	event.clear();

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

	return event;
}

// proponent and opponents' slots
std::vector<Slot> pro;
std::vector<Slot> opp;


static void
dump_card(Card const &c)
{
	if (c.is_number) {
		fprintf(stderr, "%d", c.ans);
	} else {
		fprintf(stderr, "(%s ", get_card_name(c.method));
		for (int i=0; i<c.cards.size(); i++) {
			dump_card(c.cards[i]);
		}
		fprintf(stderr, ")");
	}
}
	

void
dump_slots()
{
	for (int i=0; i<256; i++) {
		if (pro[i].root.method == CARD_I &&
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
		if (opp[i].root.method == CARD_I &&
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
