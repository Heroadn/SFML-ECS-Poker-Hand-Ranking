#include "Card.hpp"
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <optional>
#include <random>

#ifndef DECK_H
#define DECK_H

class Deck
{
public:

	Deck()
	{
		
		fill(Suit::SUIT_NUM * Rank::RANK_NUM);
	}

	~Deck()
	{
	}

	void fill(int number)
	{
		for (auto suit = 0; suit < Suit::SUIT_NUM; suit++)
		{
			for (auto rank = 0; rank < Rank::RANK_NUM; rank++)
				add(Card{ (Suit)suit, (Rank)rank });
		}
	}

	std::optional<Card> draw()
	{	
		if (empty())
			return std::nullopt;

		Card card = mCards.back();
		mCards.pop_back();
		return card;
	}

	std::vector<Card> draw(int n)
	{
		std::vector<Card> cards;

		if (empty())
			return cards;

		for (size_t i = 0; i < n; i++)
		{
			std::optional<Card> card = draw();
			if (!card.has_value())
				break;

			cards.push_back(card.value());	
		}

		return cards;
	}

	bool empty()
	{
		return mCards.empty();
	}

	void shuffle()
	{
		auto rd = std::random_device{};
		auto rng = std::default_random_engine{ rd() };
		std::shuffle(std::begin(mCards), std::end(mCards), rng);
	}

	void add(Card card)
	{
		mCards.push_back(card);
	}

private:
	std::vector<Card> mCards;
};

#endif // !DECK_H
