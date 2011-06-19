/* -*- tab-width: 4 ; indent-tabs-mode: t -*- */

#include <iostream>
#include <cstdio>
#include "solve.hpp"
#include "virtual-slot.hpp"

#define DUMP false

namespace copy_kawaii {

FILE *sim_log;

using namespace std;

#if defined(DUEL_IN_LOCAL)
#define TURN_MAX (100000)
__thread int turn_count = 0;
#else
static int turn_count = 0;
#endif

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
};

static bool
in_range(int a, int b, int c){
	return (b<=a) && (a<c);
}

Card *card_deep_copy(const Card *c)
{
	if (c->is_number) {
		return new Card(c->u.int_val);
	}

	Card *ret = new Card(c->card);
	ret->card = c->card;
	ret->is_number = false;
	ret->u.func.cur_num_applied = c->u.func.cur_num_applied;

	for (int i=0; i<c->u.func.cur_num_applied; i++) {
		ret->u.func.args[i] = card_deep_copy(c->u.func.args[i]);
	}

	return ret;
}

/*
 * もう死んでてダメージ与えられない場合はfalseを返す
 */
static bool
damage(event_list_t &events,
	   int idx,
	   enum Player pro_opp,
	   int value)
{
	int *v;
	if (pro_opp == PRO) {
		v = &pro[idx].v;
	} else {
		v = &opp[idx].v;
	}

	if (*v <= 0) {
		return false;
	}
	*v -= value;
	if (*v <= 0) {
		if (pro_opp == PRO) {
			events.push_back(Event::dead(Event::PROP_DEAD, idx));
			VSA->states[idx] = VirtualSlotAllocator::DEAD;
		}
		*v = 0;
	}

	return true;
}

static void
help(event_list_t &events,
	 int idx,
	 enum Player pro_opp,
	 int value)
{
	int *v;
	if (pro_opp == PRO) {
		v = &pro[idx].v;
	} else {
		v = &opp[idx].v;
	}

	if (*v > 0) {
		*v += value;
		if (*v > 65535) {
			*v = 65535;
		}
	}
}
	   
optional<Card *>
apply(event_list_t &events,
	  Card *func,
	  Card *arg,
	  int slot,
	  bool is_pro,
	  bool is_zombie_apply)
{
	if (func->is_number) {
		if (is_pro) {
			events.push_back(Event::type_error(slot));
		}

		return nothing();
	}

	int full_num_arg = num_arg[func->card];
	int remain = full_num_arg - func->u.func.cur_num_applied;

	assert(remain != 0);

	if (remain == 1) {
		func->u.func.args[full_num_arg-1] = arg;

		/* all arguments are applied. fold it */
		Card *x, *h,*y,*f,*g,*n, *m, *i, *j;
		optional<Card*> oh, oy, oz;

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

			oh = apply(events, f, x, slot, is_pro, is_zombie_apply);
			if (!oh) {
				return nothing();
			}
			h = *oh;

			oy = apply(events, g, x, slot, is_pro, is_zombie_apply);
			if (!oy) {
				return nothing();
			}
			y = *oy;

			oz = apply(events, h, y, slot, is_pro, is_zombie_apply);
			return oz;

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
			int jj = j->u.int_val;
			int nn = n->u.int_val;

			if (ii < 0 || ii >= N_SLOTS || jj < 0 || jj >= N_SLOTS) {
				// cerr << "Error: attack: 0 <= index <= 255: " << ii << ", " << jj << endl;
				return nothing();
			}

			if (!is_zombie_apply) {
				if (is_pro) {
					if (damage(events, ii, PRO, nn)) {
						damage(events, 255-jj, OPP, nn*9/10);
					}
				} else {
					if (damage(events, ii, OPP, nn)) {
						damage(events, 255-jj, PRO, nn*9/10);
					}
				}
			} else {
				/* The third function in card "attack" increases the
				 * w by n*9/10 if w>0
				 * (w is set to 65535 if it would become greater than 65535 by this increase),
				 * or does nothing if w<=0.
				 */
				if (is_pro) {
					if (damage(events, ii, PRO, nn)) {
						help(events, 255-jj, OPP, nn*9/10);
					}
				} else {
					if (damage(events, ii, OPP, nn)) {
						help(events, 255-jj, PRO, nn*9/10);
					}
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
			int jj = j->u.int_val;
			int nn = n->u.int_val;

			if (ii < 0 || ii >= N_SLOTS || jj < 0 || jj >= N_SLOTS) {
				// cerr << "Error: attack: 0 <= index <= 255: " << ii << ", " << jj << endl;
				return nothing();
			}

			if (!is_zombie_apply) {
				if (is_pro) {
					if (damage(events, ii, PRO, nn)) {
						help(events, jj, PRO, nn*11/10);
					}
				} else {
					if (damage(events, ii, OPP, nn)) {
						help(events, jj, OPP, nn*11/10);
					}
				}
			} else {
				if (is_pro) {
					if (damage(events, ii, PRO, nn)) {
						damage(events, jj, PRO, nn*11/10);
					}
				} else {
					if (damage(events, ii, OPP, nn)) {
						damage(events, jj, OPP, nn*11/10);
					}
				}
			}
		}
			return new Card(CARD_I);

		case CARD_K: {
			////////////////////////////////////////////////////////////////////
			// K x y
			//   return x
			x = func->u.func.args[0];
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

			i = func->u.func.args[0];
			if (! i->is_number) {
				return nothing();
			}

			if (!in_range(i->u.int_val,0,256)) {
				return nothing();
			}

			if (is_pro) {
				y = pro[i->u.int_val].f;
				stats.count_used(PRO, i->u.int_val); ///
			} else {
				y = opp[i->u.int_val].f;
				stats.count_used(OPP, i->u.int_val); ///
			}

			y = card_deep_copy(y);

			return y;
		}

		case CARD_SUCC: {
			////////////////////////////////////////////////////////////////////
			// succ n
			//   m <- n + 1
			//   return m

			n = func->u.func.args[0];
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
			
			n = func->u.func.args[0];
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

			i = func->u.func.args[0];

			if (! i->is_number) {
				return nothing();
			}

			int idx = i->u.int_val;

			if (! in_range(idx, 0, 256)) {
				return nothing();
			}

			if (!is_zombie_apply) {
				if (is_pro) {
					help(events, idx, PRO, 1);
				} else {
					help(events, idx, OPP, 1);
				}
			} else {
				if (is_pro) {
					damage(events, idx, PRO, 1);
				} else {
					damage(events, idx, OPP, 1);
				}
			}
			return new Card(CARD_I);
		}

		case CARD_DEC: {
			////////////////////////////////////////////////////////////////////
			// dec i
			//   v[255-i] <- v[255-i] - 1
			//   return I

			i = func->u.func.args[0];

			if (! i->is_number) {
				return nothing();
			}

			int idx = i->u.int_val;

			if (! in_range(idx, 0, 256)) {
				return nothing();
			}

			if (!is_zombie_apply) {
				if (is_pro) {
					damage(events, 255-idx, OPP, 1);
				} else {
					damage(events, 255-idx, PRO, 1);
				}
			} else {
				if (is_pro) {
					help(events, 255-idx, OPP, 1);
				} else {
					help(events, 255-idx, PRO, 1);
				}
			}
		}
			return new Card(CARD_I);

		case CARD_COPY:
			////////////////////////////////////////////////////////////////////
			// copy i
			//   y <- f'[i]
			//   return y

			i = func->u.func.args[0];
			if (! i->is_number) {
				return nothing();
			}

			if (in_range(i->u.int_val, 0, 256)) {
				return nothing();
			}

			if (is_pro) {
				y = opp[i->u.int_val].f;
			} else {
				y = pro[i->u.int_val].f;
			}
			return card_deep_copy(y);

		case CARD_REVIVE:
			////////////////////////////////////////////////////////////////////
			// revice i
			//   v[i] <- 1
			//   return I
			i = func->u.func.args[0];
			if (! i->is_number) {
				return nothing();
			}

			if (in_range(i->u.int_val, 0, 256)) {
				return nothing();
			}

			if (is_pro) {
				if (pro[i->u.int_val].v <= 0) {
					pro[i->u.int_val].v = 1;
				}
			} else {
				if (opp[i->u.int_val].v <= 0) {
					opp[i->u.int_val].v = 1;
				}
			}
			return new Card(CARD_I);

		case CARD_ZOMBIE:
			////////////////////////////////////////////////////////////////////
			// zombie i x
			//   f'[255-i] <- x
			//   v'[255-i] < -1
			//   return I
			i = func->u.func.args[0];
			x = func->u.func.args[1];

			if (! i->is_number) {
				return nothing();
			}

			if (! in_range(i->u.int_val, 0, 256)) {
				return nothing();
			}

			if (is_pro) {
				if (opp[255-i->u.int_val].v <= 0) {
					opp[255-i->u.int_val].v = -1;
					opp[255-i->u.int_val].f = card_deep_copy(x);
				}
			} else {
				if (pro[255-i->u.int_val].v <= 0) {
					pro[255-i->u.int_val].v = -1;
					pro[255-i->u.int_val].f = card_deep_copy(x);
				}
			}
			return new Card(CARD_I);

		case CARD_I:
			////////////////////////////////////////////////////////////////////
			// I x
			//   return x

			// return x
			x = func->u.func.args[0];
			return x;

		case CARD_ZERO:
		case CARD_UNKNOWN:
			break;
		}

		assert(0);
	} else {
		func->u.func.args[func->u.func.cur_num_applied] = arg;
		func->u.func.cur_num_applied++;
		return func;
	}
}

event_list_t
apply_card(enum card_code cc,
		   enum lr_code lr,
		   int slot,
		   bool is_pro)
{
	event_list_t events;
	Slot *slt;
	bool dump = DUMP;

	/* zombie apply */
	if (is_pro) {
		for (int i=0; i<256; i++) {
			if (pro[i].v < 0) {
				apply(events, pro[i].f, new Card(CARD_I), i, true, true);
				pro[i].v = 0;
				opp[i].f = new Card(CARD_I);
			}
		}
	} else {
		for (int i=0; i<256; i++) {
			if (opp[i].v < 0) {
				apply(events, opp[i].f, new Card(CARD_I), i, false, true);
				opp[i].v = 0;
				opp[i].f = new Card(CARD_I);
			}
		}
	}

#if 0
	if (slot >= 0 && slot < N_SLOTS) {
		if (is_pro) stats.count_used(PRO, slot);
		else stats.count_used(OPP, slot);
	}
#endif

	if (dump) {
		if (is_pro) {
			if (lr== LEFT) {
				fprintf(sim_log, "pro apply %s (%d)\n",
						get_card_name(cc),
						slot);
			} else {
				fprintf(sim_log, "pro apply %d (%s)\n",
						slot,
						get_card_name(cc));
			}
		} else {
			if (lr== LEFT) {
				fprintf(sim_log, "opp apply %s (%d)\n",
						get_card_name(cc),
						slot);
			} else {
				fprintf(sim_log, "opp apply %d (%s)\n",
						slot,
						get_card_name(cc));
			}
		}
	}

	if (is_pro) {
		slt = &pro[slot];
	} else {
		slt = &opp[slot];
	}
	Card *c;

	if (cc == CARD_ZERO) {
		c = new Card(0);
	} else {
		c = new Card(cc);
	}

	optional<Card*> r;

	if (lr == LEFT) {
		r = apply(events, c, slt->f, slot, is_pro, false);
	} else {
		r = apply(events, slt->f, c, slot, is_pro, false);
	}

	if (!r) {
		fprintf(sim_log, "NativeError\n");
		slt->f = new Card(CARD_I);
	} else {
		slt->f = *r;
	}

	stats.proc();

#if 0
	///// debug
	if (!stats.pro.slots.empty()) {
		cerr << "*** pro slot size:            " << stats.pro.slots.size() << endl;
		cerr << "*** pro used (slot, num):     "
			 << stats.pro.slots[0].slot << ", " << stats.pro.slots[0].used << endl;
		/*
		cerr << "*** pro attacked (slot, num): "
			 << stats.pro.slots[0].slot << ", " << stats.pro.slots[0].attacked << endl;
		*/
	}
	if (!stats.opp.slots.empty()) {
		cerr << "*** opp slot size:            " << stats.opp.slots.size() << endl;
		cerr << "*** opp used (slot, num):     "
			 << stats.opp.slots[0].slot << ", " << stats.opp.slots[0].used << endl;
		/*
		cerr << "*** opp attacked (slot, num): "
			 << stats.opp.slots[0].slot << ", " << stats.opp.slots[0].attacked << endl;
		*/
	}
#endif

#if defined(DUEL_IN_LOCAL)
    turn_count++;
    if (TURN_MAX <= turn_count)
    {
        pthread_exit(NULL);
    }
#else
	if (is_pro) {
		turn_count++;
	}
#endif

	return events;
}

// proponent and opponents' slots
std::vector<Slot> pro;
std::vector<Slot> opp;

static void
dump_card(Card const *c)
{
	if (c->is_number) {
		fprintf(sim_log, "%d ", c->u.int_val);
	} else {
		fprintf(sim_log, "(%s ", get_card_name(c->card));
		for (int i=0; i<c->u.func.cur_num_applied; i++) {
			dump_card(c->u.func.args[i]);
		}
		fprintf(sim_log, ")");
	}
}

static bool
is_disp_omit(Slot &slt)
{
	Card *card = slt.f;
	return (! card->is_number) &&
		(card->card == CARD_I) &&
		(slt.v == 10000);
}

void
dump_slots()
{
	fprintf(sim_log, "turn=%d\n", turn_count);
	for (int i=0; i<256; i++) {
		if (is_disp_omit(pro[i])) {
			/* omitted */
		} else {
			fprintf(sim_log, "pro%d: v=%d, f=", i, pro[i].v);
			dump_card(pro[i].f);
			fprintf(sim_log, "\n");
		}
	}

	for (int i=0; i<256; i++) {
		if (is_disp_omit(opp[i])) {
			/* omitted */
		} else {
			fprintf(sim_log, "opp%d: v=%d, f=", i, opp[i].v);
			dump_card(opp[i].f);
			fprintf(sim_log, "\n");
		}
	}
	fflush(sim_log);
}

}
