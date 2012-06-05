#ifndef CYCLE_H_
#define CYCLE_H_

#include <map>
#include <deque>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "helpers.hpp"

class Cycle
{
	float speed;
	float decay;
	float backitup;
	std::deque<sf::RectangleShape *> trail;
	sf::RectangleShape edge;
	sf::Text ready_text;
	sf::Text score_text;
	void set_edge_pos();
	void new_segment(const v2f & pos, float dir);
public:
	int score;
	std::map<Cycle *, int> deaths;
	bool ready;
	int joystick;
	v2f start;
	float startd;
	sf::Color color; // TODO
	bool crashed;
	static const float WIDTH;
	static const float SPEED;
	static const float DECAY;

	Cycle(const v2f & pos, const float dir, const sf::Color & clr, int j = -1);
	~Cycle();
	const std::deque<sf::RectangleShape *> & get_trail() const { return trail; }
	const sf::RectangleShape & get_edge() const { return edge; }
	void set_speed(const float sp) { speed = sp; }
	void set_decay(const float dc) { decay = dc; }
	void move_forward(float);
	bool shorten_trail(float);
	void move(float);
	void turn(float);
	bool check_collision(Cycle & cycle);
	bool in(const sf::RectangleShape &);
	void crash(float dist = 0.f);
	void draw(sf::RenderWindow &, bool paused) const;
	void bind(const sf::Event & event);
	void reset();
	void set_ready(bool val);
	void set_color(const sf::Color & col);
	void add_death(Cycle *cycle);
	void scored();
	void set_text_pos(const v2f & center);
	void backup();
};

#endif
