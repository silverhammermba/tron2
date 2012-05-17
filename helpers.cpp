#include "helpers.hpp"

bool perpendicular(float x, float y)
{
	return ((x ==  0.f || x == 180.f) && (y == 90.f || y == 270.f)) ||
	       ((x == 90.f || x == 270.f) && (y ==  0.f || y == 180.f));
}
