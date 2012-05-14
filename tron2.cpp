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

	sf::Text winner;
	winner.setCharacterSize(24);
	std::ostringstream win_s;

	bool paused {false};

	sf::RenderWindow window
	{
		sf::VideoMode(800, 600, 32),
		"Light Cycles",
		sf::Style::Titlebar
	};
	window.setVerticalSyncEnabled(true);

	sf::View view (window.getView());

	sf::Event event;

	sf::RectangleShape bounds {sf::Vector2f(790.f, 590.f)};
	bounds.setPosition(5.f, 5.f);
	bounds.setFillColor(sf::Color(30, 30, 30));

	Cycle *player1;
	Cycle *player2;

	player1 = new Cycle(sf::Vector2f(800.f, 300.f), 180.f, sf::Color(255, 0, 0));
	player2 = new Cycle(sf::Vector2f(0.f, 300.f), 0.f, sf::Color(0, 255, 0));

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
				// TODO center
				winner.setPosition(view.getCenter());
			}
			else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::R))
			{
				delete player1;
				delete player2;
				player1 = new Cycle(sf::Vector2f(800.f, 300.f), 180.f, sf::Color(255, 0, 0));
				player2 = new Cycle(sf::Vector2f(0.f, 300.f), 0.f, sf::Color(0, 255, 0));
				paused = false;
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
		player1->in(bounds);
		player2->in(bounds);

		// TODO generalize
		if (!paused)
		{
			win_s.str("");
			if (player1->crashed && player2->crashed)
			{
				win_s << "DRAW";
				paused = true;
			}
			else if (player1->crashed)
			{
				player2->crashed = true;
				win_s << "Player 2 wins!";
				paused = true;
			}
			else if (player2->crashed)
			{
				player1->crashed = true;
				win_s << "Player 1 wins!";
				paused = true;
			}
			winner.setString(win_s.str());
		}

		fps_s.str("");
		fps_s << "FPS " << int (1.f / time);
		fps.setString(fps_s.str());

		window.clear(sf::Color(255, 255, 255));

		window.draw(bounds);

		player1->draw(window);
		player2->draw(window);

		window.draw(fps);
		window.draw(winner);

		window.display();
	}

	return EXIT_SUCCESS;
}
