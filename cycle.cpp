#include "cycle.hpp"
#include "helpers.hpp"

// TODO debugging
#include <iostream>
using std::cerr;
using std::endl;

const float Cycle::WIDTH = 10.f;
const float Cycle::SPEED = 250.f;
const float Cycle::DECAY = Cycle::Cycle::SPEED / 2.f;

Cycle::Cycle(const sf::Vector2f & pos, const float dir, const sf::Color & clr) :
	color(clr), edge(sf::Vector2f(1.f, Cycle::WIDTH))
{
	crashed = false;
	speed = Cycle::SPEED;
	decay = Cycle::DECAY;
	trail.push_front(new sf::RectangleShape(sf::Vector2f(Cycle::WIDTH, Cycle::WIDTH)));
	trail.front()->setPosition(pos);
	trail.front()->setRotation(dir);
	trail.front()->setOrigin(Cycle::WIDTH / 2.f, Cycle::WIDTH / 2.f);
	trail.front()->setFillColor(color);

	edge.setOrigin(0.5f, Cycle::WIDTH / 2.f);
	edge.setFillColor(sf::Color(255.f, 255.f, 255.f));
	set_edge_pos();
	edge.setRotation(dir);
}

Cycle::~Cycle()
{
	delete trail.front();
}

bool Cycle::move_forward(float time)
{
	trail.front()->setSize(trail.front()->getSize() + sf::Vector2f(speed * time, 0));
	set_edge_pos();
	sf::FloatRect head = edge.getGlobalBounds();
	for (int i = 3; i < trail.size(); i++)
	{
		if (head.intersects(trail[i]->getGlobalBounds())) return false;
	}
	return true;
}

void Cycle::set_edge_pos()
{
	float dir = trail.front()->getRotation() * M_PI / 180.f;
	edge.setPosition(trail.front()->getPosition() + sf::Vector2f(std::cos(dir) , std::sin(dir)) * (trail.front()->getSize().x - Cycle::WIDTH / 2.f));
}

// returns true if the last tail segment needs to be removed
bool Cycle::shorten_trail(float time)
{
	// TODO refactor, fix tails getting too short (when paused)
	float delta = trail.back()->getSize().x - Cycle::WIDTH;
	float rad = trail.back()->getRotation() * M_PI / 180.f;
	if (delta > 0)
	{
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
	if (!crashed)
	{
		float org = trail.front()->getRotation();
		// TODO smarter way to do this? also, only restrict turns for 180s
		if (trail.front()->getSize().x > 2 * Cycle::WIDTH && perpendicular(org, dir))
		{
			float rad = org * M_PI / 180.f;
			sf::Vector2f shift (sf::Vector2f(cos(rad), sin(rad)) * (trail.front()->getSize().x - Cycle::WIDTH));
			sf::Vector2f pos (trail.front()->getPosition() + shift);

			trail.push_front(new sf::RectangleShape(sf::Vector2f(Cycle::WIDTH, Cycle::WIDTH)));
			trail.front()->setOrigin(Cycle::WIDTH / 2.f, Cycle::WIDTH / 2.f);
			trail.front()->setPosition(pos);
			trail.front()->setRotation(dir);
			trail.front()->setFillColor(color);
			edge.setRotation(dir);
		}
	}
}

bool Cycle::check_collision(Cycle & cycle)
{
	if (crashed || &cycle == this) return false;
	sf::FloatRect head = edge.getGlobalBounds();
	sf::FloatRect other = cycle.get_edge().getGlobalBounds();
	// edge case: perpendicular head-on collision
	if (head.intersects(other))
	{
		float dir = trail.front()->getRotation();
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
				//cerr << this << " crashed in edge case\n";
				crash(prg1);
				return true;
			}
			else
			{
				//cerr << this << " survived edge case (" << prg1 << " > " << prg2 << ")\n";
				return false;
			}
		}
	}
	for (auto segment : cycle.get_trail())
	{
		if (head.intersects(segment->getGlobalBounds()))
		{
			// TODO figure out how far to move back
			crash();
			//cerr << this << " crashed in normal case\n";
			return true;
		}
	}
	return false;
}

bool Cycle::in(const sf::RectangleShape & bounds)
{
	if (edge.getGlobalBounds().intersects(bounds.getGlobalBounds()))
	{
		return true;
	}
	else
	{
		crash();
		return false;
	}
}

void Cycle::crash(float dist)
{
	// back up to point of collision
	// TODO adjust wiggle distance?
	trail.front()->setSize(trail.front()->getSize() - sf::Vector2f(dist, 0));
	set_edge_pos();
	crashed = true;
}

void Cycle::draw(sf::RenderWindow & window) const
{
	for (auto rect : trail) window.draw(*rect);
	window.draw(edge);
}
