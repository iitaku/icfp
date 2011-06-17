#include <iostream>
#include <queue>
#include <string>
#include <stdlib.h>

#include <cmath>
#include <stdint.h>

using namespace std;

#define C2S (1)
#define S2C (2)

int msb(uint32_t num)
{
    if (0 == num)
    {
        return 0;
    }

    uint32_t mask = 0x80000000;
    int cnt = 0;

    while(!(mask & num))
    {
        num = num << 1;
        cnt++;
    }
    
    return 31-cnt;
}

struct Deck
{
    int mode;
    int slot;
    string card;

    Deck(int mode_, int slot_, string card_)
        : mode(mode_), slot(slot_), card(card_) {}
};

//queue<Deck> attack_slot0(uint32_t num)
//{
//    queue<Deck> q;
//
//    q.push
//}

queue<Deck> mk_num_slot0(uint32_t num)
{
    queue<Deck> q;

    q.push(Deck(S2C, 0, "zero"));
    if (num == 0)
    {
        return q;
    }
    q.push(Deck(C2S, 0, "succ"));

    int shift = msb(num);
    for (int i=0; i<shift; ++i)
    {
        q.push(Deck(C2S, 0, "dbl"));
    }

    int now_num = 1 << shift;

    for (int i=0; i<num-now_num; ++i)
    {
        q.push(Deck(C2S, 0, "succ"));
    }

    return q;
}

queue<Deck> dec_slot255()
{
    queue<Deck> q;
    q.push(Deck(S2C, 0, "zero"));
    q.push(Deck(C2S, 0, "dec"));
    return q;
}

queue<Deck> dec_slotN(uint32_t num)
{
    queue<Deck> q = mk_num_slot0(num);
    q.push(Deck(C2S, 0, "dec"));
    return q;
}

void rd(void)
{
    int mode;
    int slot;
    string card;
   
    cin >> mode;

    switch(mode)
    {
        case C2S:
            cin >> card;
            cin >> slot;
            break;
        case S2C:
            cin >> slot;
            cin >> card;
            break;
        default:
            exit (-1);
            break;
    }
}

void wt(Deck deck)
{
    cout << deck.mode << endl;

    switch(deck.mode)
    {
        case C2S:
            cout << deck.card << endl;
            cout << deck.slot << endl;
            break;
        case S2C:
            cout << deck.slot << endl;
            cout << deck.card << endl;
            break;
        default:
            break;
    }
}

int main2(int arc, char* argv[])
{
    int retval = msb(atoi(argv[1]));

    cout << retval << endl;

    return 0;
}

int main(int argc, char* argv[])
{
    if (2 != argc)
    {
        cerr << "./bot turn [turn : 0 or 1]" << endl;
        return -1;
    }

    int turn = atoi(argv[1]);
   
    /* 相手から */
    if (turn)
    {
        rd();
    }
    
    int cnt = 0;

    queue<Deck> deckq = dec_slotN(cnt);
       
    while (true)
    {
        if (deckq.empty())
        {
            //cnt++;
            deckq = dec_slotN(cnt);
        }
        
        wt(deckq.front());
        deckq.pop();
        
        rd();
    }

    return 0;
}
