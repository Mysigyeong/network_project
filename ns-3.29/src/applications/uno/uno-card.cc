
#include "uno-card.h"

using namespace std;

Uno::Uno()
{
    for(int i=1; i<5; i++)
    {
        //색상 선택
        int color;
        if(i==RED){color = RED;}
        else if(i==YELLOW){color = YELLOW;}
        else if(i==BLUE){color = BLUE;}
        else color = GREEN;
        
        //0번 카드는 색당 1장씩
        card c; c.color=color; c.number=0;
        deck.push_back(c);

        //1~9번 카드 색당 2장씩
        for(int j=1; j<10; j++)
        {
            card c1, c2;
            c1.color=color; c1.number = j;
            c2.color=color; c2.number= j;
            deck.push_back(c1);
            deck.push_back(c2);
        }

        //스페셜 카드
        for(int j=10; j<13; j++)
        {
            int special;
            if(j==SKIP){special=SKIP;}
            else if(j==REVERSE){special=REVERSE;}
            else special=DRAW_TWO;

            card c1,c2;
            c1.color = color; c1.number=special;
            c2.color = color; c2.number=special;
            deck.push_back(c1);
            deck.push_back(c2);
        }
    }

    //셔플
    shuffle();
}

void Uno::shuffle()
{
    random_shuffle(deck.begin(), deck.end());
}

card Uno::draw()
{
    //만약 deck이 비어 있다면 카드의 숫자든 색깔이든 무조건 -1
    if(deck.size()==0)
    {
        card c;
        c.color = -1;
        c.number = -1;
        return c;
    }

    card c = deck.back();
    deck.pop_back();
    return c;
}

