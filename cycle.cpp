#include "cycle.hpp"

const float Cycle::WIDTH = 10.f;
const float Cycle::SPEED = 250.f;
const float Cycle::DECAY = Cycle::Cycle::SPEED / 2.f;

Cycle::Cycle(const sf::Vector2f & pos, const float dir, const sf::Color & clr) : color(clr)
{
	crashed = false;
	speed = Cycle::SPEED;
	decay = Cycle::DECAY;
	trail.push_front(new sf::RectangleShape(sf::Vector2f(Cycle::WIDTH, Cycle::WIDTH)));
	trail.front()->setPosition(pos);
	trail.front()->setRotation(dir);
	trail.front()->setOrigin(Cycle::WIDTH / 2.f, Cycle::WIDTH / 2.f);
	trail.front()->setFillColor(color);
}

Cycle::~Cycle()
{
	delete trail.front();
}

bool Cycle::move_forward(float time)
{
	trail.front()->setSize(trail.front()->getSize() + sf::Vector2f(speed * time, 0));
	sf::FloatRect head = trail.front()->getGlobalBounds();
	for (int i = 3; i < trail.size(); i++)
	{
		if (head.intersects(trail[i]->getGlobalBounds())) return false;
	}
	return true;
}

// returns true if the last tail segment needs to be removed
bool Cycle::shorten_trail(float time)
{
	if (trail.back()->getSize().x > Cycle::WIDTH)
	{
		float rad = trail.back()->getRotation() * M_PI / 180.f;
		trail.back()->move(cos(rad) * decay * time, sin(rad) * decay * time);
		trail.back()->setSize(trail.back()->getSize() - sf::Vector2f(decay * time, 0.f));
	}
	return trail.back()->getSize().x <= Cycle::WIDTH;
}

void Cycle::move(float time)
{
	if (!crashed)
	{
		if (!move_forward(time))
			crash();
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

bool Cycle::check_collision(Cycle & cycle)
{
	sf::FloatRect head = trail.front()->getGlobalBounds();
	for (auto segment : cycle.getTrail())
	{
		if (head.intersects(segment->getGlobalBounds()))
		{
			crash();
			return true;
		}
	}
	return false;
}

void Cycle::crash()
{
	crashed = true;
}

void Cycle::draw(sf::RenderWindow & window) const
{
	for (auto rect : trail) window.draw(*rect);
}
