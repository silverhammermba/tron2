#include <map>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "cycle.hpp"
#include "helpers.hpp"

// TODO debugging
#include <iostream>
using std::cerr;
using std::endl;

const float Cycle::WIDTH = 8.5f;
const float Cycle::SPEED = 250.f;
const float Cycle::DECAY = Cycle::SPEED / 2.f;

Cycle::Cycle(const v2f & pos, const float dir, const sf::Color & clr, sf::Font& font, int j) :
	start(pos), color(clr), edge(v2f(1.f, Cycle::WIDTH)), ready_text("READY", font, 16), score_text("", font, 16)
{
	pending = -1.f; // store turns
	backitup = 0.f; // distance to back up after collision
	score = 0;
	joystick = j;
	startd = dir; // starting direction

	edge.setOrigin(0.5f, Cycle::WIDTH / 2.f);
	edge.setFillColor(sf::Color(255.f, 255.f, 255.f));

	sf::FloatRect size = ready_text.getGlobalBounds();
	ready_text.setOrigin(size.width / 2, size.height / 2);
	ready_text.setColor(sf::Color(0, 0, 0));

	// get ready for first round
	reset();
}

Cycle::~Cycle()
{
	delete trail.front();
}

// move and check for self-collision
void Cycle::move_forward(float time)
{
	// move forward
	trail.front()->setSize(trail.front()->getSize() + v2f(speed * time, 0));
	set_edge_pos();
	sf::FloatRect head = edge.getGlobalBounds();
	float dir = trail.front()->getRotation();
	// check for self-collision
	// it's not possible to collide with the first three segments
	for (int i = 3; i < trail.size(); i++)
	{
		sf::FloatRect rect = trail[i]->getGlobalBounds();
		if (head.intersects(rect))
		{
			float dist;
			if (dir == 0.f)
				dist = head.left + head.width - rect.left;
			else if (dir == 90.f)
				dist = head.top + head.height - rect.top;
			else if (dir == 180.f)
				dist = rect.left + rect.width - head.left;
			else if (dir == 270.f)
				dist = rect.top + rect.height - head.top;
			crash(dist);
			add_death(this);
		}
	}
}

// update position of leading edge
void Cycle::set_edge_pos()
{
	float dir = trail.front()->getRotation() * M_PI / 180.f;
	edge.setPosition(trail.front()->getPosition() + v2f(std::cos(dir) , std::sin(dir)) * (trail.front()->getSize().x - Cycle::WIDTH / 2.f));
}

// shorten the trail
// returns true if the last tail segment needs to be removed
bool Cycle::shorten_trail(float time)
{
	// TODO refactor, fix tails getting too short (when paused)
	float delta = trail.back()->getSize().x - Cycle::WIDTH;
	float rad = trail.back()->getRotation() * M_PI / 180.f;
	if (delta > 0)
	{
		trail.back()->move(cos(rad) * decay * time, sin(rad) * decay * time);
		trail.back()->setSize(trail.back()->getSize() - v2f(decay * time, 0.f));
	}
	return trail.back()->getSize().x <= Cycle::WIDTH;
}

// move the player
void Cycle::move(float time)
{
	if (!crashed)
	{
		// check for stored turn
		if (pending >= 0.f)
			turn(pending);
		move_forward(time);
	}
	if (shorten_trail(time) && trail.size() > 1)
	{
		delete trail.back();
		trail.pop_back();
	}
}

// turn to the direction dir (absolute)
void Cycle::turn(float dir)
{
	if (!crashed)
	{
		float org = trail.front()->getRotation();
		// TODO allow quick doglegs
		if (perpendicular(org, dir))
		{
			if (trail.front()->getSize().x > 2 * Cycle::WIDTH)
			{
				float rad = org * M_PI / 180.f;
				v2f shift (v2f(cos(rad), sin(rad)) * (trail.front()->getSize().x - Cycle::WIDTH));
				v2f pos (trail.front()->getPosition() + shift);

				new_segment(pos, dir);
				// clear any stored turn
				pending = -1.f;
			}
			// store the turn for later
			else
				pending = dir;
		}
	}
}

// check for a collision between this and cycle
bool Cycle::check_collision(Cycle& cycle)
{
	if (crashed || &cycle == this) return false;
	sf::FloatRect head = edge.getGlobalBounds();
	sf::FloatRect other = cycle.get_edge().getGlobalBounds();
	float dir = trail.front()->getRotation();
	// edge case: perpendicular head-on collision
	if (head.intersects(other))
	{
		float oth = cycle.get_trail().front()->getRotation();
		if (perpendicular(dir, oth))
		{
			float prg1, prg2;

			if (dir == 0.f) // 1 right
				prg1 = head.left - other.left;
			else if (dir == 90.f) // 1 down
				prg1 = head.top - other.top;
			else if (dir == 180.f) // 1 left
				prg1 = other.width - head.left + other.left;
			else // 1 up
				prg1 = other.height - head.top + other.top;

			if (oth == 0.f) // 2 right
				prg2 = other.left - head.left;
			else if (oth == 90.f) // 2 down
				prg2 = other.top - head.top;
			else if (oth == 180.f) // 2 left
				prg2 = head.width - other.left + head.left;
			else // 2 up
				prg2 = head.height - other.top + head.top;

			if (prg1 <= prg2)
			{
				crash(prg1);
				add_death(&cycle);
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	// normal collision
	for (auto segment : cycle.get_trail())
	{
		sf::FloatRect rect = segment->getGlobalBounds();
		if (head.intersects(rect))
		{
			// determine distance to back up
			float dist;
			if (dir == 0.f)
				dist = head.left + head.width - rect.left;
			else if (dir == 90.f)
				dist = head.top + head.height - rect.top;
			else if (dir == 180.f)
				dist = rect.left + rect.width - head.left;
			else if (dir == 270.f)
				dist = rect.top + rect.height - head.top;
			crash(dist);
			add_death(&cycle);
			return true;
		}
	}
	return false;
}

// check if the player is in bounds
bool Cycle::in(const sf::RectangleShape & bounds)
{
	sf::FloatRect head = edge.getGlobalBounds();
	sf::FloatRect bound = bounds.getGlobalBounds();
	if (head.intersects(bound))
		return true;
	else
	{
		// determine amount to back up
		float dir = edge.getRotation();
		float dist;
		if (dir == 0.f)
			dist = head.left + head.width - bound.left - bound.width;
		else if (dir == 90.f)
			dist = head.top + head.height - bound.top - bound.height;
		else if (dir == 180.f)
			dist = bound.left - head.left;
		else if (dir == 270.f)
			dist = bound.top - head.top;
		crash(dist);
		add_death(NULL);
		return false;
	}
}

// crash the player where dist is the amount of penetration
// (to be backed up later)
// TODO track life time
void Cycle::crash(float dist)
{
	backitup = dist;
	crashed = true;
}

// back up after a collision
void Cycle::backup()
{
	if (crashed)
	{
		trail.front()->setSize(trail.front()->getSize() - v2f(backitup, 0));
		set_edge_pos();
		backitup = 0.f;
	}
}

// draw the player on the screen
void Cycle::draw(sf::RenderWindow & window, bool paused) const
{
	for (auto rect : trail)
		window.draw(*rect);
	window.draw(edge);
	if (paused)
	{
		window.draw(ready_text);
		window.draw(score_text);
	}
}

// respond to an event
void Cycle::bind(const sf::Event & event)
{
	if (joystick >= 0)
	{
		if (event.type == sf::Event::JoystickButtonPressed && event.joystickButton.joystickId == joystick)
		{
			switch (event.joystickButton.button)
			{
				case 0:
					turn(90.f);
					break;
				case 1:
					turn(0.f);
					break;
				case 2:
					turn(180.f);
					break;
				case 3:
					turn(270.f);
					break;
				case 7:
					set_ready(true);
					break;
				case 8:
					set_ready(true);
					break;
			}
		}
	}
	// TODO subcontrols
	else if (event.type == sf::Event::KeyPressed)
	{
		switch (event.key.code)
		{
			case sf::Keyboard::D:
				turn(0.f);
				break;
			case sf::Keyboard::S:
				turn(90.f);
				break;
			case sf::Keyboard::A:
				turn(180.f);
				break;
			case sf::Keyboard::W:
				turn(270.f);
				break;
			case sf::Keyboard::Return:
				set_ready(true);
				break;
			case sf::Keyboard::BackSpace:
				set_ready(false);
				break;
		}
	}
}

// set the player back to their starting position with a
// single segment
void Cycle::reset()
{
	ready = false;
	crashed = false;
	speed = Cycle::SPEED;
	decay = Cycle::DECAY;
	trail.clear();
	new_segment(start, startd);
	set_edge_pos();
}

// add a new segment to the front of the snake
void Cycle::new_segment(const v2f & pos, float dir)
{
	trail.push_front(new sf::RectangleShape(v2f(Cycle::WIDTH, Cycle::WIDTH)));
	trail.front()->setPosition(pos);
	trail.front()->setRotation(dir);
	trail.front()->setOrigin(Cycle::WIDTH / 2.f, Cycle::WIDTH / 2.f);
	trail.front()->setFillColor(color);

	edge.setRotation(dir);
}

// setter for ready status
void Cycle::set_ready(bool val)
{
	ready = val;
	if (val)
		ready_text.setColor(color);
	else
		ready_text.setColor(sf::Color(0, 0, 0));
}

// change trail color
void Cycle::set_color(const sf::Color & col)
{
	color = col;
	for (auto segment : trail)
		segment->setFillColor(color);
	if (ready)
		ready_text.setColor(color);
}

// track deaths per-player
// TODO display somehow
void Cycle::add_death(Cycle *cycle)
{
	if (deaths.count(cycle))
		deaths[cycle]++;
	else
		deaths[cycle] = 1;
}

// increment score
void Cycle::scored()
{
	score++;
	std::ostringstream text;
	text << score;
	score_text.setString(text.str());
}

// update position of ready/score text using center of view
void Cycle::set_text_pos(const v2f & center)
{
	v2f new_pos {(start.x + center.x) / 2.f, (start.y + center.y) / 2.f};
	ready_text.setPosition(new_pos);
	score_text.setPosition(new_pos + v2f(20, 0));
}
