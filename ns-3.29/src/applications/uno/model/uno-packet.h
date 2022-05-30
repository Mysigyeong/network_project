#ifndef UNO_PACKET_H
#define UNO_PACKET_H

#include "uno-card.h"
enum class GameOp : uint32_t
{
    INIT,     // Game initialization.
    TURN,     // Turn start
    DRAW,     // Draw Cards
    UNO,      // Users have to say uno
    UNODRAW,  // UNO is too late
    GAMEOVER  // Gameover
};

enum class UserOp : uint32_t
{
    PLAY,     // Play a card
    DRAW      // Draw a card; the user cannot play a card
};


struct UnoPacket
{
    uint32_t    seq;                // Sequence number
    uint32_t    uid;                // User ID
    GameOp      gameOp;             // Game operation code
    UserOp      userOp;             // User operation code
    uint32_t    playing;            // who's turn?
    // Please add any fields if you need.
    uint32_t    numOfCards;         // Number of cards in the packet
    card        cards[10];          // Server to Client;
    card        frontcard;          // Front Card on the Deck
    card        passingcard;        // Card Passing;
    uint32_t    color;              // Selected color; used for wildcard
};



#endif
