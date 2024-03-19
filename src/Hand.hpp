#include "Card.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <unordered_set>	
#include <string>

#ifndef HAND_H
#define HAND_H

namespace Ranking
{
	enum Hand
	{
		STRAIGHT,
		STRAIGHT_FLUSH,
		FOUR_OF_A_KIND,
		THREE_OF_A_KIND,
		FULL_HOUSE,
		FLUSH,
		ROYAL_FLUSH,
		ONE_PAIR,
		TWO_PAIR,
		HIGHEST,
		NOT_ENOUGH_CARDS
	};


	std::string toString(Hand hand) {
		switch (hand) {
		case Hand::STRAIGHT:
			return "Straight";
		case Hand::STRAIGHT_FLUSH:
			return "Straight Flush";
		case Hand::FOUR_OF_A_KIND:
			return "Four of a Kind";
		case Hand::THREE_OF_A_KIND:
			return "Three of a Kind";
		case Hand::FULL_HOUSE:
			return "Full House";
		case Hand::FLUSH:
			return "Flush";
		case Hand::ROYAL_FLUSH:
			return "Royal Flush";
		case Hand::ONE_PAIR:
			return "One Pair";
		case Hand::TWO_PAIR:
			return "Two Pair";
		case Hand::HIGHEST:
			return "Highest";
		case Hand::NOT_ENOUGH_CARDS:
			return "Not enough cards";
		default:
			return "Unknown Hand";
		}
	}
	
	Card highestCard(std::vector<Card> cards)
	{
		Card highest = cards[0];

		for (auto card : cards)
		{
			if (card.rank == Rank::ACE)
				return card;
			if (card.rank > highest.rank)
				highest = card;
		}

		return highest;
	}
	
	bool isFlush(std::vector<Card> cards)
	{
		std::unordered_map<Suit, int> count;

		//counting number of suits
		for (Card card : cards)
		{
			count[card.suit]++;
			if (count[card.suit] == cards.size())
				return true;
		}

		return false;
	}

	bool isStraight(std::vector<Card> cards)
	{
		//Five cards in order, such as 2♠-3♡-4♢-5♠-6♣. The suits do not match.
		std::sort(cards.begin(), cards.end(), [cards](Card a, Card b) {
			return a.rank < b.rank;
		});

		std::unordered_map<Suit, int> count;
		int lowest = cards[0].rank;

		//searching for a sequence
		for (Card card : cards)
		{
			if (card.rank != lowest++)
				return false;
		}

		//counting number of suits
		for (Card card : cards)
		{
			count[card.suit]++;
			if (count[card.suit] > 2)
				return true;
		}

		return true;
	}
	
	bool isStraightFlush(std::vector<Card> cards)
	{
		return isStraight(cards) && isFlush(cards);
	}

	bool isFourOfAKind(std::vector<Card> cards)
	{
		std::unordered_map<Rank, int> count;

		//counting number of ranks
		for (Card card : cards)
			count[card.rank]++;

		for (auto [key, value] : count)
			if (value > 4)
				return true;

		return false;
	}

	bool isThreeOfAKind(std::vector<Card> cards)
	{
		std::unordered_map<Rank, int> count;

		//counting number of ranks
		for (Card card : cards)
			count[card.rank]++;

		for (auto [key, value] : count)
			if (value == 3)
				return true;

		return false;
	}
	
	int countPairs(std::vector<Card> cards)
	{
		std::unordered_map<Rank, int> count;
		std::vector<Rank> pairs;

		//counting number of ranks
		for (Card card : cards)
			count[card.rank]++;

		//are two cards the same rank
		for (auto [key, value] : count)
			if (value == 2)
				pairs.push_back(key);

		return pairs.size();
	}

	bool isTwoPair(std::vector<Card> cards)
	{
		return countPairs(cards) == 2;
	}
	
	bool isAPair(std::vector<Card> cards)
	{
		return countPairs(cards) == 1;
	}

	bool isFullHouse(std::vector<Card> cards)
	{
		return isThreeOfAKind(cards) && isAPair(cards);
	}
	
	bool isRoyalFlush(std::vector<Card> cards)
	{
		std::unordered_set<Rank> search = {
			Rank::ACE,
			Rank::KING,
			Rank::QUEEN,
			Rank::JACK,
			Rank::TEN};

		
		//searching for the cards
		for (Card card : cards)
		{
			auto it = search.find(card.rank);

			//return false if rank is not on the list
			if (it == search.end())
				return false;
		}

		return isFlush(cards);
	}
	
	Hand ranking(std::vector<Card> cards)
	{
		Hand result = Hand::HIGHEST;

		if (cards.size() < 5)
			return Hand::NOT_ENOUGH_CARDS;
		
		if (Ranking::isRoyalFlush(cards))
		{
			return Hand::ROYAL_FLUSH;
		}

		if (Ranking::isStraightFlush(cards))
		{
			return Hand::STRAIGHT_FLUSH;
		}

		if (Ranking::isFourOfAKind(cards))
		{
			return Hand::FOUR_OF_A_KIND;
		}

		if (Ranking::isFullHouse(cards))
		{
			return Hand::FULL_HOUSE;
		}

		if (Ranking::isFlush(cards))
		{
			return Hand::FLUSH;
		}

		if (Ranking::isStraight(cards))
		{
			return Hand::STRAIGHT;
		}

		if (Ranking::isThreeOfAKind(cards))
		{
			return Hand::THREE_OF_A_KIND;
		}

		if (Ranking::isTwoPair(cards))
		{
			return Hand::TWO_PAIR;
		}

		if (Ranking::isAPair(cards))
		{
			return Hand::ONE_PAIR;
		}

		return result;
	}

	void print(std::vector<Card> cards)
	{
		std::cout << "Ranking: " << Ranking::toString(Ranking::ranking(cards)) << std::endl;
		for (Card card : cards) {
			std::cout << card.toString();
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
}

#endif // !HAND_H