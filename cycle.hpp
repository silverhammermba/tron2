#ifndef CYCLE_H_
#define CYCLE_H_

#include <deque>
#include <SFML/Graphics.hpp>

class Cycle
{
	int dx, dy;
	float speed;
	float decay;
	bool crashed;
	sf::Color color;
	std::deque<sf::RectangleShape *> trail;
public:
	static const float WIDTH;
	static const float SPEED;
	static const float DECAY;
	Cycle(const sf::Vector2f & pos, const float dir, const sf::Color & clr);
	~Cycle();
	const std::deque<sf::RectangleShape *> & getTrail() const { return trail; }
	void set_speed(const float sp) { speed = sp; }
	void set_decay(const float dc) { decay = dc; }
	bool move_forward(float);
	bool shorten_trail(float);
	void move(float);
	void turn(float);
	bool check_collision(Cycle & cycle);
	void crash();
	void draw(sf::RenderWindow &) const;
};

#endif
