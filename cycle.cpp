#include "cycle.hpp"

const float Cycle::WIDTH = 10.f;
const float Cycle::SPEED = 250.f;
const float Cycle::DECAY = Cycle::Cycle::SPEED / 2.f;

Cycle::Cycle(const sf::Vector2f & pos, const float dir, const sf::Color & clr) : color(clr)
{
	speed = Cycle::SPEED;
	decay = Cycle::DECAY;
	trail.push_front(new sf::RectangleShape(sf::Vector2f(Cycle::WIDTH, Cycle::WIDTH)));
	trail.front()->setPosition(pos);
	trail.front()->setRotation(dir);
	trail.front()->setOrigin(Cycle::WIDTH / 2.f, Cycle::WIDTH / 2.f);
	trail.front()->setFillColor(color);
}

void Cycle::move_forward(float time)
{
	trail.front()->setSize(trail.front()->getSize() + sf::Vector2f(speed * time, 0));
}

// returns true if the last tail segment needs to be removed
bool Cycle::shorten_trail(float time)
{
	float rad = trail.back()->getRotation() * M_PI / 180.f;
	trail.back()->move(cos(rad) * decay * time, sin(rad) * decay * time);
	trail.back()->setSize(trail.back()->getSize() - sf::Vector2f(decay * time, 0.f));
	return trail.back()->getSize().x <= Cycle::WIDTH;
}

void Cycle::move(float time)
{
	move_forward(time);
	if (shorten_trail(time)) trail.pop_back();
}

// turn to the direction dir (absolute)
void Cycle::turn(float dir)
{
	float org = trail.front()->getRotation();
	// TODO smarter way to do this?
	if (trail.front()->getSize().x > 2 * Cycle::WIDTH &&
	   (((org ==  0.f || org == 180.f) && (dir == 90.f || dir == 270.f)) ||
	    ((org == 90.f || org == 270.f) && (dir ==  0.f || dir == 180.f))))
	{
		float rad = org * M_PI / 180.f;
		sf::Vector2f shift (sf::Vector2f(cos(rad), sin(rad)) * (trail.front()->getSize().x - Cycle::WIDTH));
		sf::Vector2f pos (trail.front()->getPosition() + shift);

		trail.push_front(new sf::RectangleShape(sf::Vector2f(Cycle::WIDTH, Cycle::WIDTH)));
		trail.front()->setOrigin(Cycle::WIDTH / 2.f, Cycle::WIDTH / 2.f);
		trail.front()->setPosition(pos);
		trail.front()->setRotation(dir);
		trail.front()->setFillColor(color);
	}
}

void Cycle::draw(sf::RenderWindow & window) const
{
	for (auto rect : trail) window.draw(*rect);
}
