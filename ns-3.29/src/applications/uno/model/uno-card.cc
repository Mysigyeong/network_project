#include "ns3/log.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv6-address.h"

#include "ns3/nstime.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"

#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"

#include "uno-card.h"
#include "uno-packet.h"
#include "uno-server.h"

using namespace std;
using namespace ns3;
Uno::Uno()
{
    turn = 0;
    playing = 0;
    player_No = 0;
    color = 1;

    
    for(uint32_t temp_color = 1; temp_color < 5; temp_color++)
    {
        //0번 카드는 색당 1장씩
        card c;
        c.color = temp_color;
        c.number = 0;
        deck.push_back(c);

        //1~9번 카드 및 스페셜 카드 색당 2장씩
        for(uint32_t j = 1; j < 13; j++)
        {
            card c1, c2;
            c1.color = temp_color; c1.number = j;
            c2.color = temp_color; c2.number = j;
            deck.push_back(c1);
            deck.push_back(c2);
        }
    }

    // 4 Wild cards
    for(uint32_t j=0; j<4; j++)
    {
        card c1,c2;
        c1.color=SPECIAL; c1.number=WILD;
        c2.color=SPECIAL; c2.number=WILD_DRAW_FOUR;
        deck.push_back(c1);
        deck.push_back(c2);
    }
    //셔플
    Shuffle();
}

void Uno::Shuffle()
{
    random_device rd;
    default_random_engine rng(rd());
    shuffle(deck.begin(), deck.end(), rng);
}

card Uno::Draw()
{
    //만약 deck에 남은 카드가 없다면
    if(deck.size()==0)
    {
        if(trash_deck.size()==0)
        {
            card empty_card;
            empty_card.color=0;
            empty_card.number=0;
            {cout<<"Unable to draw card from now: NO DECK LEFT"<<endl;}
            return empty_card;
        }
        Trash2Deck();
    }

    card c = deck.back();
    deck.pop_back();
    return c;
}

void Uno::Collect_Trash(card c)
{
    if (c.color < 0 || c.color > 4) {
        cout << "Card color is out of range" << endl;
        return;
    }

    if (c.color == 0) {
        if (!(c.number == WILD || c.number == WILD_DRAW_FOUR)) {
            cout << "Invalid card" << endl;
            return;
        }
    }
    else {
        if (c.number > 12) {
            cout << "Card number is out of range" << endl;
            return;
        }
    }

    trash_deck.push_back(c);
}

void Uno::Trash2Deck()
{
    deck.clear();
    deck.assign(trash_deck.begin(), trash_deck.end());
    Shuffle();
    trash_deck.clear();
}