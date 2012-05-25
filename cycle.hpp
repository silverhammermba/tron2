#ifndef CYCLE_H_
#define CYCLE_H_

#include <deque>
#include <SFML/Graphics.hpp>
#include "helpers.hpp"

class Cycle
{
	float speed;
	float decay;
	std::deque<sf::RectangleShape *> trail;
	sf::RectangleShape edge;
	void set_edge_pos();
public:
	sf::Color color; // TODO
	bool crashed;
	static const float WIDTH;
	static const float SPEED;
	static const float DECAY;

	Cycle(const v2f & pos, const float dir, const sf::Color & clr);
	~Cycle();
	const std::deque<sf::RectangleShape *> & get_trail() const { return trail; }
	const sf::RectangleShape & get_edge() const { return edge; }
	void set_speed(const float sp) { speed = sp; }
	void set_decay(const float dc) { decay = dc; }
	bool move_forward(float);
	bool shorten_trail(float);
	void move(float);
	void turn(float);
	bool check_collision(Cycle & cycle);
	bool in(const sf::RectangleShape &);
	void crash(float dist = 0.f);
	void draw(sf::RenderWindow &) const;
};

#endif
