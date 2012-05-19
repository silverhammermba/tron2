#include "helpers.hpp"

bool perpendicular(float x, float y)
{
	return ((x ==  0.f || x == 180.f) && (y == 90.f || y == 270.f)) ||
	       ((x == 90.f || x == 270.f) && (y ==  0.f || y == 180.f));
}

float dist2(const sf::Vector2f & v, const sf::Vector2f & w)
{
	return (v.x - w.x) * (v.x - w.x) + (v.y - w.y) * (v.y - w.y);
}
