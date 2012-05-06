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

	Cycle *player1;
	Cycle *player2;

	player1 = new Cycle(sf::Vector2f(400.f, 0.f), 90.f, sf::Color(255, 0, 0));
	player2 = new Cycle(sf::Vector2f(100.f, 300.f), 0.f, sf::Color(0, 255, 0));

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
			else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::R))
			{
				delete player1;
				delete player2;
				player1 = new Cycle(sf::Vector2f(400.f, 0.f), 90.f, sf::Color(255, 0, 0));
				player2 = new Cycle(sf::Vector2f(100.f, 300.f), 0.f, sf::Color(0, 255, 0));
			}
		}

		// TODO real controls
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			player1->turn(0.f);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			player1->turn(90.f);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			player1->turn(180.f);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			player1->turn(270.f);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			player2->turn(0.f);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			player2->turn(90.f);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			player2->turn(180.f);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			player2->turn(270.f);
		}

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		player1->move(time);
		player2->move(time);
		player1->check_collision(*player2);
		player2->check_collision(*player1);

		if (player1->crashed)
			player2->crashed = true;
		if (player2->crashed)
			player1->crashed = true;

		fps_s.str("");
		fps_s << "FPS " << int (1.f / time);
		fps.setString(fps_s.str());

		window.clear(sf::Color(30, 30, 30));

		player1->draw(window);
		player2->draw(window);

		window.draw(fps);

		window.display();
	}

	return EXIT_SUCCESS;
}
