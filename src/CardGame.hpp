#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <unordered_map>
#include <entt/entt.hpp>
#include "Card.hpp"
#include "Deck.hpp"
#include <functional>

#ifndef CARDGAME_H
#define CARDGAME_H

const int WIDTH = 640;
const int HEIGHT = 480;

enum Direction { None, Up, Down, Left, Right };
Direction direction;
entt::entity player;
std::vector<std::vector<sf::IntRect>> cardsFrames;

//
entt::registry registry;

struct PlayerComponent { bool controllable = true; };

struct SpriteComponent : public sf::Sprite {
	SpriteComponent(
		std::string path,
		sf::Vector2f position,
		sf::IntRect rect = {},
		float scale = 1.0f,
		float speed = 1.0f
	)
	{
		load(path);
		setFrame(rect);
		setScale(scale, scale);
		setPosition(position);
		mSpeed = speed;
	}

	sf::Image mImage;
	sf::Texture mTexture;
	sf::IntRect mFrame;
	float mSpeed;

	void setFrame(int left, int top, int width, int height)
	{
		mFrame = sf::IntRect(left, top, width, height);
		setTextureRect(mFrame);
	}

	void setFrame(sf::IntRect& mFrame)
	{
		mFrame = mFrame;
		setTextureRect(mFrame);
	}

	void load(std::string path, int width = 0, int heigth = 0, sf::Color color = sf::Color::Transparent)
	{
		try
		{
			if (!mImage.loadFromFile(ASSETS_PATH + path))
				throw std::runtime_error("Could not load image: " ASSETS_PATH + path);
		}
		catch (const std::exception& e) {
			std::cerr << "Exception: " << e.what() << std::endl;
		}

		mTexture.loadFromImage(mImage);
		setTexture(mTexture);
	}

	void move(Direction direction, float dt)
	{
		auto position = getPosition();
		sf::Vector2f movement{ 0.f, 0.f };

		switch (direction)
		{
		case None:
			break;
		case Up:
			movement.y -= mSpeed;
			break;
		case Down:
			movement.y += mSpeed;
			break;
		case Left:
			movement.x -= mSpeed;
			break;
		case Right:
			movement.x += mSpeed;
			break;
		default:
			break;
		}

		position += movement * dt;
		setPosition(position);
	}
};

struct AnimatorComponent
{
	struct Animation
	{
		Animation(std::vector<sf::IntRect> frames, float speed)
			: frames(frames), speed(speed) {
		}

		Animation() = default;

		std::vector<sf::IntRect> frames;
		sf::IntRect mFrame;

		float speed = 0;
		float current = 0;
		int index = 0;
		
		void addFrame(sf::IntRect frame) {
			frames.emplace_back(std::move(frame));
		}
	};

	AnimatorComponent(
		std::unordered_map <std::string, Animation> animations, std::string start)
	{
		mAnimations = animations;
		change(start);
	}

	std::unordered_map <std::string, Animation> mAnimations;
	Animation mAnimation;

	void change(std::string& name)
	{
		auto it = mAnimations.find(name);

		if (it != mAnimations.end()) {
			mAnimation = it->second;
			mAnimation.index = 0;
		}
	}

	const sf::IntRect& update(float dt)
	{
		mAnimation.current += dt;

		if (mAnimation.current > mAnimation.speed)
		{
			//reseting acumulator and incrementing mFrame
			mAnimation.index = (mAnimation.index + 1) % mAnimation.frames.size();
			mAnimation.current = 0;

			//
			mAnimation.mFrame = mAnimation.frames[mAnimation.index];
		}

		return mAnimation.mFrame;
	}
};

struct HitBoxComponent
{
	HitBoxComponent(sf::FloatRect rect)
	{
		setHitbox(rect);
	}

	void setHitbox(sf::FloatRect hitbox)
	{
		mHitbox = hitbox;
	}

	sf::FloatRect getHitbox()
	{
		return mHitbox;
	}

	sf::FloatRect mHitbox;
};

struct ButtonComponent
{
	ButtonComponent(
		std::function<void(entt::entity entity)> onAction)
		: onAction(onAction)
	{
	}

	ButtonComponent()
		: onAction([](entt::entity entity) {})
	{
	}

	std::function<void(entt::entity entity)> onAction;
};

struct CardComponent : Card
{
	CardComponent(Card card = { Suit::HEART, Rank::ACE })
	{
		suit = card.suit;
		rank = card.rank;
	}
};

struct HandComponent
{
	struct Slot
	{
		Card card{ Suit::HEART, Rank::ACE};
		entt::entity entity = entt::null;
	};

	HandComponent(std::vector<Card> cards, std::vector<std::vector<sf::IntRect>>& frames)
		:mFrames(frames), visible(false)
	{
		fill(cards);
	}

	void fill(std::vector<Card> cards)
	{
		auto offset = 0.0f;
		std::string spriteSheet = "Cards_Pixel_Art/white_black.png";

		for (auto& card : cards)
		{
			auto& frame = mFrames[Suit::DIAMONDS][Rank::ACE];
			auto x = 200.0f + (64.0f * (offset++));
			auto y = 350.0f;

			//creating cards to purple player
			auto entity = registry.create();
			registry.emplace<CardComponent>(entity, card);
			registry.emplace<SpriteComponent>(entity, spriteSheet, sf::Vector2f{ x, y }, frame, 1.0f);
			registry.emplace<HitBoxComponent>(entity, sf::FloatRect{ x, y, 64.0f, 96.0f });
			registry.emplace<ButtonComponent>(entity, [this](entt::entity entity)
			{
				std::cout << ";)" << std::endl;
				remove(entity);
			});
		}
	}

	void reveal()
	{
		//searching cards
		auto view = registry.view<CardComponent, SpriteComponent>();

		view.each([this](auto& card, auto& sprite) {
			sprite.setFrame(mFrames[card.suit][card.rank]);
		});

		visible = true;
	}

	void hide()
	{
		//searching cards
		auto view = registry.view<CardComponent, SpriteComponent>();

		view.each([this](auto& card, auto& sprite) {
			sprite.setFrame(mFrames[Suit::DIAMONDS][Rank::ACE]);
		});

		visible = false;
	}

	void remove(entt::entity entity)
	{
		//registry.remove<SpriteComponent>(entity);
		registry.destroy(entity);
	}

	bool isHide()
	{
		return !visible;
	}

	std::vector<std::vector<sf::IntRect>> mFrames;
	bool visible;
};

//adicionar observer que ao entidade button ser clicada ele retorna a entidade clicada

void populateCardFrames(std::vector<std::vector<sf::IntRect>>& cardsFrames)
{
	for (auto suit = 0; suit < Suit::SUIT_NUM; suit++)
	{
		cardsFrames.emplace_back();
		auto& frames = cardsFrames.back();

		//
		for (auto rank = 0; rank < Rank::RANK_NUM; rank++)
			frames.emplace_back(rank * 64, suit * 96, 64, 96);
	}

}

/*
[](entt::entity entity)
	{
		std::cout << ";)" << std::endl;
	};
*/

class CardGame
{
public:
	CardGame()
		: window(sf::VideoMode(WIDTH, HEIGHT), "SFML Demo")
	{
	
	}

	~CardGame()
	{
	}

	void init()
	{
		Deck deck;
		deck.shuffle();
		Card card = deck.draw().value();
		populateCardFrames(cardsFrames);
		std::vector<Card> cards = deck.draw(5);

		//
		std::unordered_map<std::string, AnimatorComponent::Animation> mAnimations;
		mAnimations["moving"] = {
			AnimatorComponent::Animation({
			sf::IntRect{ 34 * 0, 0, 34, 34 },
			sf::IntRect{ 34 * 1, 0, 34, 34 },
			sf::IntRect{ 34 * 2, 0, 34, 34 },
			sf::IntRect{ 34 * 3, 0, 34, 34 }}, 0.1f) };

		float middleWidth = WIDTH / 2;
		float middleHeight = HEIGHT / 2;

		auto player = registry.create();
		registry.emplace<SpriteComponent>(player, "bat.png", sf::Vector2f{ 10.0f, 10.0f }, sf::IntRect{ 34 * 0, 0, 34, 34 }, 2.0f, 300.0f);//171, 103, sf::Color{ 111, 109, 81 };
		registry.emplace<AnimatorComponent>(player, mAnimations, "moving");
		registry.emplace<PlayerComponent>(player);
		registry.emplace<HandComponent>(player, cards, cardsFrames);
		registry.emplace<HitBoxComponent>(player, sf::FloatRect{ 10.0f, 10.0f, 34, 34 });

		auto buttonW = registry.create();
		registry.emplace<SpriteComponent>(buttonW, "Gerald_Keys/Keys/W-Key.png", sf::Vector2f{ middleWidth, middleHeight - 30 }, sf::IntRect{ 0, 0, 32, 32 }, 2.0f);

		auto buttonS = registry.create();
		registry.emplace<SpriteComponent>(buttonS, "Gerald_Keys/Keys/S-Key.png", sf::Vector2f{ middleWidth, middleHeight + 64 - 30 }, sf::IntRect{ 0, 0, 32, 32 }, 2.0f);

		auto buttonD = registry.create();
		registry.emplace<SpriteComponent>(buttonD, "Gerald_Keys/Keys/D-Key.png", sf::Vector2f{ middleWidth + 64, middleHeight }, sf::IntRect{ 0, 0, 32, 32 }, 2.0f);

		auto buttonA = registry.create();
		registry.emplace<SpriteComponent>(buttonA, "Gerald_Keys/Keys/A-Key.png", sf::Vector2f{ middleWidth - 64, middleHeight }, sf::IntRect{ 0, 0, 32, 32 }, 2.0f, 2.0f);

		std::unordered_map<std::string, AnimatorComponent::Animation> cardAnimation;
		cardAnimation["default"] = {
			AnimatorComponent::Animation(cardsFrames[card.suit], 0.5f) };

		auto cardAnimating = registry.create();
		registry.emplace<SpriteComponent>(cardAnimating, "Cards_Pixel_Art/white_black.png", sf::Vector2f{ 100, 100 }, cardsFrames[card.suit][card.rank], 2.0f);
		registry.emplace<AnimatorComponent>(cardAnimating, cardAnimation, "default");
	}

	void run()
	{
		init();
		sf::Event event;
		sf::Clock delta;
		float dt = delta.restart().asSeconds();

		//
		while (window.isOpen()){
			input(event);
			update(dt);

			window.clear(sf::Color::White);
			render();
			window.display();

			dt = delta.restart().asSeconds();
		}
	}

	void update(float dt)
	{
		auto& sprite = registry.get<SpriteComponent>(player);
		sprite.move(direction, dt);

		auto animationsView = registry.view<AnimatorComponent, SpriteComponent>();
		animationsView.each([&dt](auto& animator, auto& sprite) {
			//updating animation
			const auto& [left, top, width, height] = animator.update(dt);
			sprite.setFrame(left, top, width, height);
		});

		auto buttonView = registry.view<HitBoxComponent, ButtonComponent>();
		buttonView.each([&dt, this](const auto entity, auto& hitbox, auto& button) {
			sf::Vector2f position = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			auto result = hitbox.getHitbox().contains(position);

			if (result && sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				button.onAction(entity);
			}
		});
	}

	void input(sf::Event& event)
	{
		while (window.pollEvent(event)) {
			//Handle events
			if (event.type == sf::Event::EventType::Closed) {
				window.close();
			}

			if (event.type == sf::Event::EventType::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
				}
			}
		}

		direction = Direction::None;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			direction = Direction::Left;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			direction = Direction::Right;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			direction = Direction::Up;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			direction = Direction::Down;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			//
			auto handView = registry.view<HandComponent>();
			handView.each([](auto& hand)
			{
					if (hand.isHide())
						hand.reveal();
					else
						hand.hide();
			});
		}

		auto handView = registry.view<HandComponent>();
		/*
		handView.each([&selectedCard](auto& hand)
		{
			sf::Vector2f position = window.mapPixelToCoords(sf::Mouse::getPosition(window));

			for (auto& [key, {entity, card}] : hand.mCards)
			{
				auto& sprite = registry.get<SpriteComponent>(entity);
				auto result = sprite.getHitbox().contains(position);

				if (result && sf::Mouse::isButtonPressed(sf::Mouse::Left))
					selectedCard = key;
			}
		});

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace)) {

			//
			auto handView = registry.view<HandComponent>();
			handView.each([&selectedCard](auto& hand)
			{
				//trocar para queue ou stack de cartas a serem removidas
				//assim trocando select card por uma estrutura que enquanto
				//nao vazia apenas remova
				if (selectedCard != -1)
					hand.remove(selectedCard);

				selectedCard = -1;
			});
		}*/
	}

	void render()
	{
		auto view = registry.view<SpriteComponent>();
		view.each([this](auto& sprite) {
			window.draw(sprite);
		});
	}
private:
	sf::RenderWindow window;
};

#endif // !CARDGAME_H
