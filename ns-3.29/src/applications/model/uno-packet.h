#ifndef UNO_PACKET_H
#define UNO_PACKET_H

enum class GameOp : uint32_t
{
    INIT,     // Game initialization.
    TURN,     // Turn start
    PENALTY,  // A user should draw cards
    UNO,      // Users have to say uno
    GAMEOVER  // Gameover
};

enum class UserOp : uint32_t
{
    PLAY,     // Play a card
    DRAW,     // Draw a card; the user cannot play a card
    UNO       // Say uno
};

enum class Color : uint32_t
{
    BLUE,
    GREEN,
    RED,
    YELLOW
};

struct Card
{
    Color color;
    uint32_t num;
};

struct UnoPacket
{
    uint32_t seq;         // Sequence number
    
    uint32_t uid;         // User ID
    GameOp gameOp;        // Game operation code
    UserOp userOp;        // User operation code

    // Please add any fields if you need.
    uint32_t numOfCards;  // Number of cards in the packet
    Card cards[10];       // List of cards
    Color color;          // Selected color; used for wildcard
};

#endif
