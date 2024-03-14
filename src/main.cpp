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

int main(int argc, char** argv) {

    CardGame game;
    game.run();
}

sf::Image removeColor(sf::Image mImage,int width, int height, int r, int g, int b) {

    for (int y = 0; y < height; y++) {
        //removing pixels
        for (int x = 0; x < width; x++) {
            int pr = mImage.getPixel(x, y).r == r;
            int pg = mImage.getPixel(x, y).g == g;
            int pb = mImage.getPixel(x, y).b == b;

            if (pr  && pg  && pb ) {    
                 mImage.setPixel(x, y, sf::Color(0,0,0,0));
            }
        }
    }

    return mImage;
}

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