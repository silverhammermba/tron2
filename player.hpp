#ifndef PLAYER_H_
#define PLAYER_H_

#include <SFML/Graphics.hpp>
#include "cycle.hpp";

class Player
{
	sf::Color color;
	// TODO controls
public:
	Cycle *cycle;

	Player(const sf::Color & clr);
	~Player();
	void set_controls();
};

#endif
