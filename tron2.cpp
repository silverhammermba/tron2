#include <iostream>
#include <sstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include "cycle.hpp"

#define _USE_MATH_DEFINES

using std::cerr;
using std::endl;

int main(int argc, char *argv[])
{
	sf::Clock clock;

	sf::Text fps;
	fps.setCharacterSize(12);
	std::ostringstream fps_s;

	sf::RenderWindow window
	{
		sf::VideoMode(800, 600, 32),
		"Light Cycles",
		sf::Style::Titlebar
	};
	window.setVerticalSyncEnabled(true);

	sf::View view (window.getView());

	sf::Event event;

	Cycle player (sf::Vector2f(50.f, 300.f), 0.f, sf::Color(255, 0, 0));
	sf::Clock t;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (
				event.type == sf::Event::Closed ||
				((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Escape))
				)
				window.close();
			else if (event.type == sf::Event::Resized)
			{
				view.setSize((sf::Vector2f)window.getSize());
				window.setView(view);
				fps.setPosition(view.getCenter() - view.getSize() / 2.f);
			}
		}

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		float testt = t.getElapsedTime().asSeconds();
		if (testt > 1.5f)
		{
			t.restart();
			player.turn(90.f);
		}

		player.move(time);

		fps_s.str("");
		fps_s << "FPS " << int (1.f / time);
		fps.setString(fps_s.str());

		window.clear(sf::Color(30, 30, 30));

		player.draw(window);

		window.draw(fps);

		window.display();
	}

	return EXIT_SUCCESS;
}
