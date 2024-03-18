#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <unordered_map>
#include <entt/entt.hpp>
#include <functional>
#include "Card.hpp"
#include "Deck.hpp"
#include "Hand.hpp"

#ifndef CARDGAME_H
#define CARDGAME_H

const int WIDTH = 640;
const int HEIGHT = 480;

const int M_WIDTH  = WIDTH / 2;
const int M_HEIGHT = HEIGHT / 2;

enum Direction { None, Up, Down, Left, Right };
Direction direction;

std::unordered_map<int, std::vector<sf::IntRect>> cardsFrames;
entt::registry registry;

class TextureManager
{
public:
	std::shared_ptr<sf::Texture> loadTexture(std::string path, sf::Color background = sf::Color::Transparent)
	{
		//
		sf::Image image;

		//return cached texture
		auto it = mTextures.find(path);
		if (it != mTextures.end())
		{
			return it->second;
		}

		//load and cache texture
		try
		{
			if (!image.loadFromFile(ASSETS_PATH + path))
				throw std::runtime_error("Could not load image: " ASSETS_PATH + path);
		}
		catch (const std::exception& e) {
			std::cerr << "Exception: " << e.what() << std::endl;
			std::exit(1);
		}

		mTextures[path] = std::make_shared<sf::Texture>();
		mTextures[path]->loadFromImage(image);
		return mTextures[path];
	}

	sf::Image removeColor(sf::Image image, sf::Color color) {
		auto [width, height] = image.getSize();

		for (int y = 0; y < height; y++) {
			//removing pixels
			for (int x = 0; x < width; x++) {
				int pr = image.getPixel(x, y).r == color.r;
				int pg = image.getPixel(x, y).g == color.g;
				int pb = image.getPixel(x, y).b == color.b;

				if (pr && pg && pb) {
					image.setPixel(x, y, sf::Color(0, 0, 0, 0));
				}
			}
		}

		return image;
	}

private:
	std::unordered_map<std::string, std::shared_ptr<sf::Texture>> mTextures;
};

TextureManager manager;
sf::Clock timeout;
Deck deck;
Deck select;

struct PlayerComponent { bool controllable = true; };

struct TextComponent
{
	TextComponent(std::string str, sf::Font& font, sf::Color color, sf::Vector2f position, int size)
		: mFont(font)
	{
		mText.setFont(mFont);
		mText.setString(str);
		mText.setCharacterSize(size);
		mText.setFillColor(color);
		mText.setPosition(position);
	}

	sf::FloatRect localBounds()
	{
		return mText.getGlobalBounds();
	}

	void draw(sf::RenderWindow& window)
	{
		window.draw(mText);
	}

	sf::Text mText;
	sf::Font mFont;
};

struct SpriteComponent : public sf::Sprite {
	SpriteComponent(
		std::string path,
		sf::Vector2f position,
		sf::IntRect rect = {},
		float scale = 1.0f,
		float speed = 1.0f
	)
	{
		setTexture(*manager.loadTexture(path));
		setFrame(rect);
		setScale(scale, scale);
		setPosition(position);
		mSpeed = speed;
		spriteSheet = path;
	}
	
	void load(std::string path, int width = 0, int heigth = 0)
	{
		try
		{
			if (!image.loadFromFile(ASSETS_PATH + path))
				throw std::runtime_error("Could not load image: " ASSETS_PATH + path);
		}
		catch (const std::exception& e) {
			std::cerr << "Exception: " << e.what() << std::endl;
		}

		texture.loadFromImage(image);
		setTexture(texture);
	}

	//
	sf::Texture texture;
	sf::Image image;

	sf::IntRect mFrame;
	std::string spriteSheet;
	float mSpeed;

	sf::FloatRect localBounds()
	{
		return getGlobalBounds();
	}

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
		: mOnAction(onAction)
	{
	}

	ButtonComponent()
		: mOnAction([](entt::entity entity) {})
	{
	}

	void onAction(entt::entity entity)
	{
		if (clock.getElapsedTime().asSeconds() > cooldown)
		{
			mOnAction(entity);
			clock.restart();
		}
	}

	std::function<void(entt::entity entity)> mOnAction;
	sf::Clock clock;
	float cooldown = 0.2f;
};

struct CardComponent : Card
{
	CardComponent(Card card = { Suit::HEART, Rank::ACE })
	{
		suit = card.suit;
		rank = card.rank;
	}

	~CardComponent()
	{
		//std::cout << "DELETING CARD" << std::endl;
	}
};

struct HandComponent
{
	struct Slot
	{
		Card card{ Suit::HEART, Rank::ACE};
		entt::entity entity = entt::null;
	};

	HandComponent(std::vector<Card> cards, std::unordered_map<int, std::vector<sf::IntRect>>& frames)
		:mFrames(frames), visible(false)
	{
		fill(cards);
	}

	void fill(std::vector<Card> cards)
	{
		auto offset = 0.0f;

		//avaliable positions to place cards
		for (auto& card : cards)
		{
			//auto x = (WIDTH / 4) + (64.0f * (offset++)) - (offset * 15.0f);
			auto x = (WIDTH / 4) + (64.0f * (offset++));
			auto y = 370.0f;
			mPositions.push_back({x, y});
			mAvaliable.push_back({ x, y });
		}

		//adding cards
		for (auto& card : cards)
			addCard(card);

		visible = true;
	}

	void refresh(std::vector<Card> cards)
	{
		auto view = registry.view<CardComponent, SpriteComponent>();

		//removing cards
		view.each([this](auto entity, auto& card, auto& sprite) {
			remove(entity);
		});

		//adding cards
		for (Card card : cards)
		{
			addCard(card);
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

	void addCard(Card card)
	{
		if (isFull() || isEmpty())
			return;

		std::string black = "Cards_Pixel_Art/white_black.png";
		std::string red   = "Cards_Pixel_Art/white_red.png";

		//auto& frame = mFrames[Suit::HEART][Rank::QUEEN];
		auto& frame = mFrames[card.suit][card.rank];
		auto [x, y] = mAvaliable.back();
		mAvaliable.pop_back();

		auto& spriteSheet = (card.isRed()) ? red : black;
		//std::cout << card.toString() << std::endl;

		//creating cards to purple player
		auto entity = registry.create();
		registry.emplace<CardComponent>(entity, card);
		registry.emplace<SpriteComponent>(entity, spriteSheet, sf::Vector2f{ x, y }, frame, 1.0f);
		registry.emplace<HitBoxComponent>(entity, sf::FloatRect{ x, y, 64.0f, 96.0f });
		registry.emplace<ButtonComponent>(entity, [this](entt::entity entity)
		{
			remove(entity);
		});

		n++;
	}

	void remove(entt::entity entity)
	{
		auto& sprite = registry.get<SpriteComponent>(entity);
		mAvaliable.push_back(sprite.getPosition());
		registry.destroy(entity);
		n--;
	}

	const std::string& rank()
	{
		auto view = registry.view<CardComponent>();
		std::vector<Card> cards;

		//verify if theres 5 cards
		view.each([this, &cards](auto& card) {
			cards.push_back(card);
		});

		if (cards.size() < 5)
			return "";

		auto result = Ranking::ranking(cards);
		rankStr = Ranking::toString(result);
		Ranking::print(cards);
		return rankStr;
	}

	bool isHide()
	{
		return !visible;
	}

	bool isFull()
	{
		return mAvaliable.size() > 5;
	}

	bool isEmpty()
	{
		return mAvaliable.empty();
	}

	std::unordered_map<int, std::vector<sf::IntRect>> mFrames;
	std::vector<sf::Vector2f> mPositions;
	std::vector<sf::Vector2f> mAvaliable;

	bool visible;
	int max = 5;
	int n = 0;
	std::string rankStr = "";
};

void populateCardFrames(std::unordered_map<int, std::vector<sf::IntRect>>& cardsFrames)
{
	//order of suits in spritesheet
	std::vector<int> order = {Suit::CLUBS, Suit::HEART, Suit::SPADES, Suit::DIAMONDS};

	for (auto suit = 0; suit < Suit::SUIT_NUM; suit++)
	{
		std::vector<sf::IntRect> frames;
		for (auto rank = 0; rank < Rank::RANK_NUM; rank++)
			frames.emplace_back(rank * 64, suit * 96, 64, 96);

		cardsFrames[order[suit]] = frames;
	}
}

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
		deck.shuffle();
		populateCardFrames(cardsFrames);

		//
		auto hand = registry.create();
		registry.emplace<HandComponent>(hand, deck.draw(5), cardsFrames);

		//std::string& str, sf::Font font, sf::Color color, int size
		sf::Font font;
		if (!font.loadFromFile(ASSETS_PATH"NotJamFontPack/Not Jam Laika 11/Not Jam Laika 11.ttf"))
			std::cout << "ERROR WHILE LOADING FONT" << std::endl;

		//JOKER_VERMELHO
		registry.emplace<TextComponent>(
			registry.create(), "SELECT YOUR HAND", font, sf::Color::Red, sf::Vector2f{ 10, 8 }, 30);

		//
		auto eye = registry.create();
		auto& eyeSprite = registry.emplace<SpriteComponent>(eye, "Pixel Icons/1bit 16px icons part-2 outlines.png", sf::Vector2f{ 490, 375 }, sf::IntRect{81, 80, 16, 16}, 2.5f);
		registry.emplace<HitBoxComponent>(eye, eyeSprite.localBounds());
		registry.emplace<ButtonComponent>(eye, [this](entt::entity entity)
		{
			auto handView = registry.view<HandComponent>();
			auto& sprite = registry.get<SpriteComponent>(entity);

			handView.each([&sprite](auto& hand)
				{
					if (hand.isHide())
					{
						hand.reveal();
						sprite.setFrame(sf::IntRect{ 81, 80, 16, 16 });
					}
					else
					{
						hand.hide();
						sprite.setFrame(sf::IntRect{ 104, 80, 16, 16 });
					}
				});
		});

		auto eye1 = registry.create();
		auto& eyeSprite1 = registry.emplace<SpriteComponent>(eye1, "Pixel Icons/1bit 16px icons part-2 outlines.png", sf::Vector2f{ 490, 375 + 51 }, sf::IntRect{ 320, 128, 16, 16 }, 2.5f);
		registry.emplace<HitBoxComponent>(eye1, eyeSprite1.localBounds());
		registry.emplace<ButtonComponent>(eye1, [this](entt::entity entity)
		{
			auto handView = registry.view<HandComponent>();
			handView.each([this](auto& hand)
			{
				std::vector<Card> cards = deck.draw(5);
				hand.refresh(cards);
			});
		});

		//
		auto offsetX = 0.0f;
		auto offsetY = 205.0f;

		auto ranking = registry.create();
		auto& textRanking = registry.emplace<TextComponent>(ranking, "[RANKING]", font, sf::Color::Black, sf::Vector2f{ 10 + offsetX, 170 + offsetY }, 20);
		registry.emplace<HitBoxComponent>(ranking, textRanking.localBounds());
		registry.emplace<ButtonComponent>(ranking, [this](entt::entity entity)
			{
				std::cout << "RANKING CARD" << std::endl;
				auto handView = registry.view<HandComponent>();
				handView.each([this](auto& hand)
				{
						hand.rank();
				});
			});

		//add face that changes emotion based in hand ranking
		//auto face = registry.create();
		//registry.emplace<SpriteComponent>(face, "Cute faces.png", sf::Vector2f{ 100, 200 }, sf::IntRect{ 34, 407, 341, 334 }, 0.2f);

		//make a ballon with text
		//I have tasted the grape juice
		//and it's opened up my eyes ;)
		//reference to an infected mushroom song
		auto call = registry.create();
		auto& textAdd = registry.emplace<TextComponent>(call, "[DECK]", font, sf::Color::Black, sf::Vector2f{ 10 + offsetX, 200 + offsetY }, 20);
		registry.emplace<HitBoxComponent>(call, textAdd.localBounds());
		registry.emplace<ButtonComponent>(call, [this](entt::entity entity)
		{
			std::cout << "DECK CARD" << std::endl;
		});

		//
		auto draw = registry.create();
		auto& textDraw = registry.emplace<TextComponent>(draw, "[DRAW]", font, sf::Color::Black, sf::Vector2f{ 10 + offsetX, 230 + offsetY }, 20);
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
		});

		//make a deck component
		auto [xCard, yCard] = sf::Vector2f{ 0,0};
		auto max = 13;
		auto cardOffsetX = 8.0f;
		size_t i = 0;

		std::string black = "Cards_Pixel_Art/white_black.png";
		std::string red = "Cards_Pixel_Art/white_red.png";
		
		for (auto card : select)
		{
			xCard = (40.0f * (i % max)) + cardOffsetX;
			yCard += (i % max == 0) ? 58 : 0;
			i++;

			auto& spriteSheet = (card.isRed()) ? red : black;
			auto cardSelect = registry.create();
			auto& cardSelectSprite = registry.emplace<SpriteComponent>(cardSelect, spriteSheet, sf::Vector2f{ xCard, yCard }, cardsFrames[card.suit][card.rank], 0.6f);
			registry.emplace<HitBoxComponent>(cardSelect, cardSelectSprite.localBounds());
			registry.emplace<ButtonComponent>(cardSelect, [this, card](entt::entity entity)
			{
				auto handView = registry.view<HandComponent>();
				handView.each([&, card](auto& hand)
				{
						hand.addCard(card);
				});
				
			});
		}
		
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
				button.onAction(entity);

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
	}

	void render()
	{
		auto viewSprite = registry.view<SpriteComponent>();
		viewSprite.each([this](auto& sprite) {
			window.draw(sprite);
		});

		auto viewText = registry.view<TextComponent>();
		viewText.each([this](auto& text) {
			text.draw(window);
		});
	}
private:
	sf::RenderWindow window;
};

#endif // !CARDGAME_H
