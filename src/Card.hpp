#ifndef CARD_H
#define CARD_H

enum Rank
{
    ACE,
    TWO,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    JACK,
    QUEEN,
    KING,
    RANK_NUM
};

enum Suit {
    HEART,
    SPADES,
    CLUBS,
    DIAMONDS,
    SUIT_NUM
};

struct Card
{
    Suit suit;
    Rank rank;
};

#endif // !CARD_H
