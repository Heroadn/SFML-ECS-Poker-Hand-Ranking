/*The author of the mImage(" bat.png ")    is "Jetrel"    taken from URL(" https://opengameart.org/content/castle-platformer ") */
/*The author of the mImage keys            is "Gerald"    taken from URL(" https://gerald-burke.itch.io/geralds-keys ") */

#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <unordered_map>
#include <entt/entt.hpp>
#include "Card.hpp"
#include "Deck.hpp"
#include "CardGame.hpp"
#include "Hand.hpp"

int main(int argc, char** argv) {

    CardGame game;
    //game.run();

    std::vector<Card> straight = {
        {Suit::HEART, Rank::TEN},
        {Suit::CLUBS, Rank::NINE},
        {Suit::DIAMONDS, Rank::EIGHT},
        {Suit::SPADES, Rank::SEVEN},
        {Suit::HEART, Rank::SIX}
    };

    std::vector<Card> straightFlush = {
        {Suit::HEART, Rank::TEN},
        {Suit::HEART, Rank::NINE},
        {Suit::HEART, Rank::EIGHT},
        {Suit::HEART, Rank::SEVEN},
        {Suit::HEART, Rank::SIX}
    };

    std::vector<Card> fourOfAKind = {
        {Suit::HEART, Rank::QUEEN},
        {Suit::SPADES, Rank::QUEEN},
        {Suit::DIAMONDS, Rank::QUEEN},
        {Suit::CLUBS, Rank::QUEEN},
        {Suit::DIAMONDS, Rank::QUEEN}
    };

    std::vector<Card> threeOfAKind = {
        {Suit::HEART, Rank::QUEEN},
        {Suit::SPADES, Rank::QUEEN},
        {Suit::DIAMONDS, Rank::QUEEN},
        {Suit::CLUBS, Rank::SEVEN},
        {Suit::SPADES, Rank::SIX}
    };

    std::vector<Card> fullHouse = {
       {Suit::HEART, Rank::ACE},
       {Suit::CLUBS, Rank::ACE},
       {Suit::DIAMONDS, Rank::ACE},
       {Suit::SPADES, Rank::KING},
       {Suit::HEART, Rank::KING}
    };

    std::vector<Card> flush = {
       {Suit::CLUBS, Rank::KING},
       {Suit::CLUBS, Rank::TEN},
       {Suit::CLUBS, Rank::EIGHT},
       {Suit::CLUBS, Rank::SEVEN},
       {Suit::CLUBS, Rank::FIVE}
    };

    std::vector<Card> royalFlush = {
       {Suit::HEART, Rank::ACE},
       {Suit::HEART, Rank::KING},
       {Suit::HEART, Rank::QUEEN},
       {Suit::HEART, Rank::JACK},
       {Suit::HEART, Rank::TEN}
    };

    std::vector<Card> onePair = {
       {Suit::SPADES, Rank::QUEEN},
       {Suit::HEART, Rank::QUEEN},
       {Suit::HEART, Rank::SIX},
       {Suit::CLUBS, Rank::NINE},
       {Suit::DIAMONDS, Rank::TWO}
    };

    std::vector<Card> twoPair = {
       {Suit::HEART, Rank::JACK},
       {Suit::CLUBS, Rank::JACK},
       {Suit::DIAMONDS, Rank::NINE},
       {Suit::CLUBS, Rank::NINE},
       {Suit::DIAMONDS, Rank::TWO}
    };

    std::vector<Card> test1 = {
       {Suit::SPADES, Rank::ACE},
       {Suit::SPADES, Rank::KING},
       {Suit::SPADES, Rank::QUEEN},
       {Suit::SPADES, Rank::JACK},
       {Suit::SPADES, Rank::TEN}
    };

    std::vector<Card> test2 = {
      {Suit::HEART, Rank::KING},
      {Suit::CLUBS, Rank::KING},
      {Suit::DIAMONDS, Rank::KING},
      {Suit::CLUBS, Rank::SEVEN},
      {Suit::SPADES, Rank::NINE}
    };

    //STRAIGHT, STRAIGHT FLUSH, FOUR OF A KIND, THREE OF A KIND
    //FULL HOUSE, FLUSH, ROYAL FLUSH, ONE PAIR, TWO PAIR

    // Imprime o ranking do Straight Flush
    Ranking::print(straightFlush);

    // Imprime o ranking do Four of a Kind
    Ranking::print(fourOfAKind);

    // Imprime o ranking do Full House
    Ranking::print(fullHouse);

    // Imprime o ranking do Flush
    Ranking::print(flush);

    // Imprime o ranking do Straight
    Ranking::print(straight);

    // Imprime o ranking do Three of a Kind
    Ranking::print(threeOfAKind);

    // Imprime o ranking do Two Pair
    Ranking::print(twoPair);

    // Imprime o ranking do One Pair
    Ranking::print(onePair);

    // Test 1
    Ranking::print(test1);

    // Test 2
    Ranking::print(test2);
}

sf::Image removeColor(sf::Image mImage, int width, int height, int r, int g, int b) {

    for (int y = 0; y < height; y++) {
        //removing pixels
        for (int x = 0; x < width; x++) {
            int pr = mImage.getPixel(x, y).r == r;
            int pg = mImage.getPixel(x, y).g == g;
            int pb = mImage.getPixel(x, y).b == b;

            if (pr && pg && pb) {
                mImage.setPixel(x, y, sf::Color(0, 0, 0, 0));
            }
        }
    }

    return mImage;
}

/*
//
auto offsetX = 0.0f;
auto offsetY = 205.0f;

auto ranking = registry.create();
auto& textRanking = registry.emplace<TextComponent>(ranking, "[RANKING]", font, sf::Color::Black, sf::Vector2f{ 10 + offsetX, 170 + offsetY }, 20);
registry.emplace<HitBoxComponent>(ranking, textRanking.localBounds());
registry.emplace<ButtonComponent>(ranking, [this, &rankText](entt::entity entity)
    {
        std::cout << "RANKING CARD" << std::endl;
        auto handView = registry.view<HandComponent>();
        handView.each([this, &rankText](auto& hand)
        {
                rankText.setText(hand.rank());
        });
    });

//
auto draw = registry.create();
auto& textDraw = registry.emplace<TextComponent>(draw, "[DRAW]", font, sf::Color::Black, sf::Vector2f{ 10 + offsetX, 200 + offsetY }, 20);
registry.emplace<HitBoxComponent>(draw, textDraw.localBounds());
registry.emplace<ButtonComponent>(draw, [this](entt::entity entity)
{
    auto handView = registry.view<HandComponent>();
    handView.each([this](auto& hand)
    {
        std::optional<Card> card = deck.draw();

        if (card.has_value())
            hand.addCard(card.value());
    });
    std::cout << "DRAW CARD" << std::endl;
});*/

/*
    // If true, you will continue to receive keyboard events when a key is held down
    // If false, it will only fire one event per press until released
    window.setKeyRepeatEnabled(false);

    std::unordered_map<int, bool> keys;
    std::list<int> changedKeys;

    //RANDOW NUMBERS
    srand(time(NULL));               //seed random number generator with the current time
    auto randomNumber = rand() % 255;//generate a random number then confine it to a value of 0-255.
    std::uniform_int_distribution<int> randomColorRange(0, 255);
    std::random_device rd; //returns a random number to use as a seed for the mt algorithm
    std::mt19937 randomNumbers(rd()); //"shape" the results to our range
*/