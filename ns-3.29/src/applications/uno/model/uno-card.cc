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
    turn=0;
    playing=0;
    player_No=0;


    for(uint32_t i=1; i<5; i++)
    {
        //색상 선택
        uint32_t color;
        if(i==RED){color = RED;}
        else if(i==YELLOW){color = YELLOW;}
        else if(i==BLUE){color = BLUE;}
        else color = GREEN;
        
        //0번 카드는 색당 1장씩
        card c; c.color=color; c.number=0;
        deck.push_back(c);

        //1~9번 카드 색당 2장씩
        for(uint32_t j=1; j<10; j++)
        {
            card c1, c2;
            c1.color=color; c1.number = j;
            c2.color=color; c2.number= j;
            deck.push_back(c1);
            deck.push_back(c2);
        }

        //스페셜 카드
        for(uint32_t j=10; j<13; j++)
        {
            uint32_t special;
            if(j==SKIP){special=SKIP;}
            else if(j==REVERSE){special=REVERSE;}
            else special=DRAW_TWO;

            card c1,c2;
            c1.color = 0; c1.number=special;
            c2.color = 0; c2.number=special;
            deck.push_back(c1);
            deck.push_back(c2);
        }

        for(uint32_t j=0; j<4; j++)
        {
            card c1,c2;
            c1.color=0; c2.number=WILD;
            c2.color=0; c2.number=WILD_DRAW_FOUR;
            deck.push_back(c1);
            deck.push_back(c2);
        }
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
    if(c.color<0 || c.color>4)
    {cout<<"Card color is out of range"<<endl;}
    if(c.number<0 || c.number>14)
    {cout<<"Card number is out of range"<<endl;}
    trash_deck.push_back(c);
}

void Uno::Trash2Deck()
{
    deck.clear();
    deck.assign(trash_deck.begin(), trash_deck.end());
    Shuffle();
    trash_deck.clear();
}

/*uint32_t main()
{
    Uno uno;
    for(uint32_t i=0; i<10; i++)
    {
        card c;
        c.color = i%5;
        c.number = i;
        uno.Collect_Trash(c);
    }
    uno.Trash2Deck();
    for(uint32_t i=0; i<uno.deck.size(); i++)
    {
        cout<<"color:   "<<uno.deck.at(i).color<<endl;
        cout<<"number:   "<<uno.deck.at(i).number<<endl;
        cout<<""<<endl;
    }

    cout<<"size: "<<uno.trash_deck.size()<<endl;
    return 0;
}*/


