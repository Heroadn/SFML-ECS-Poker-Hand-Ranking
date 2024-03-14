#include <string>

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

    bool isRed()
    {
        return suit == Suit::HEART || suit == Suit::DIAMONDS;
    }

    std::string toString()
    {
        std::string result;
        std::string suitStr;
        std::string rankStr;

        toStringSuit(suitStr);
        toStringRank(rankStr);

        result = "SUIT: " + suitStr + " Rank: " + rankStr;
        return result;
    }

    void toStringSuit(std::string& suitStr)
    {
        switch (suit)
        {
        case HEART:
            suitStr = "Heart   ";
            break;
        case SPADES:
            suitStr = "Spades  ";
            break;
        case CLUBS:
            suitStr = "Clubs   ";
            break;
        case DIAMONDS:
            suitStr = "Diamonds";
            break;
        default:
            break;
        }
    }

    void toStringRank(std::string& rankStr)
    {
        switch (rank)
        {
        case ACE:
            rankStr = "ACE";
            break;
        case TWO:
            rankStr = "ACE";
            break;
        case THREE:
            rankStr = "THREE";
            break;
        case FOUR:
            rankStr = "FOUR";
            break;
        case FIVE:
            rankStr = "FIVE";
            break;
        case SIX:
            rankStr = "SIX";
            break;
        case SEVEN:
            rankStr = "SEVEN";
            break;
        case EIGHT:
            rankStr = "EIGHT";
            break;
        case NINE:
            rankStr = "NINE";
            break;
        case TEN:
            rankStr = "TEN";
            break;
        case JACK:
            rankStr = "JACK";
            break;
        case QUEEN:
            rankStr = "QUEEN";
            break;
        case KING:
            rankStr = "KING";
            break;
        case RANK_NUM:
            break;
        default:
            break;
        }
    }
};

#endif // !CARD_H
