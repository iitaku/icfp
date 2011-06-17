#include <iostream>
#include <queue>
#include <string>

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

class Strategy
{
public:    
    virtual queue<Deck> get() = 0;
};

template<uint32_t num>
class MkNumSlot0 : public Strategy
{
public:    
    virtual queue<Deck> get()
    {
        queue<Deck> q;
        
        q.push(Deck(S2C, 0, "zero"));
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
};

template<uint32_t num>
class MkNumSlotN : public Strategy
{
public:    
    virtual queue<Deck> get()
    {
        queue<Deck> q;
        
        q.push(Deck(S2C, 0, "zero"));
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
};

class DecSlot0 : public Strategy
{
public:    
    virtual queue<Deck> get()
    {
        queue<Deck> q;
        q.push(Deck(S2C, 0, "zero"));
        q.push(Deck(C2S, 0, "dec"));
        return q;
    }
};

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

    //Strategy* strategy = new DecSlot0;
    Strategy* strategy = new MkNumSlot0<255>;
   
    /* 相手から */
    if (turn)
    {
        rd();
    }
   
    queue<Deck> deckq;

    while (true)
    {
        if (deckq.empty())
        {
            deckq = strategy->get();
        }
        
        wt(deckq.front());
        deckq.pop();
        
        rd();
    }

    delete strategy;

    return 0;
}
