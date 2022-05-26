#ifndef UNO_CARD_H
#define UNO_CARD_H

#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>
#include "ns3/simulator.h"
#include "ns3/packet.h"
using namespace std;

//Normal card
//color=1,2,3,4, number=0,1,2,3,4,5,6,7,8,9
#define RED 1
#define YELLOW 2
#define BLUE 3
#define GREEN 4

//special card
//color=0,  number=10,11,12
#define SKIP 10     //다음 턴 사람 넘기고 그 다음사람으로 ㄱ
#define REVERSE 11  //순서 바뀜
#define DRAW_TWO 12 // 다음 턴 사람 두장 먹고 다음 사람으로 턴 넘김

//Joker
//color=0, number=13,14
#define WILD 13     //진행중인 카드의 색을 바꿈
#define WILD_DRAW_FOUR 14   //진행중인 카드의 색을 바꿈 + 다음 사람에게 4장 먹임

struct card{
        uint32_t color;
        uint32_t number;
};

class Uno{
    public:

    card front;  //현재 맨위의 카드
    uint32_t turn; // 턴 수
    uint32_t playing; //현재 차례인 player
    uint32_t player_No; //player 수

    vector <card> deck;
    vector <card> trash_deck;
    


    public:
    Uno();
    void Shuffle();
    card Draw();
    void Collect_Trash(card c);
    void Trash2Deck();
};
#endif
