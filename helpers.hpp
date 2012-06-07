#ifndef TRON_HELPERS_H_
#define TRON_HELPERS_H_

#include <cmath>
#include <SFML/System.hpp>

#define _USE_MATH_DEFINES

#ifndef M_PI
#define M_PI 3.141592653589793238462643
#endif

typedef sf::Vector2f v2f;

bool perpendicular(float x, float y);

// constrain x to [lb, ub]
template <class T>
T clamp(T lb, T x, T ub)
{
	return (x < lb ? lb : (x > ub ? ub : x));
}

// find distance between two points as sf::Vector2<T>
template <class T>
float v2dist(sf::Vector2<T> v1, sf::Vector2<T> v2)
{
	return std::sqrt((float)((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y)));
}

#endif
