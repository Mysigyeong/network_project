#include <iostream>
#include <stack>
#include <vector>
#include <algorithm>
#include <iterator>
#include <random>

using namespace std;

#define RED 1
#define YELLOW 2
#define BLUE 3
#define GREEN 4

#define SKIP 10     //다음 턴 사람 넘기고 그 다음사람으로 ㄱ
#define REVERSE 11  //순서 바뀜
#define DRAW_TWO 12 // 다음 턴 사람 두장 먹고 다음 사람으로 턴 넘김

struct card{
        int color;
        int number;
};

class Uno{
    private:
    
    vector <card> deck;

    public:
    Uno();
    void shuffle();
    card draw();

};