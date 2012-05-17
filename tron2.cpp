#include <iostream>
#include <sstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
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
	Cycle *player3;
	Cycle *player4;

	player1 = new Cycle(sf::Vector2f(695.f, 300.f), 180.f, sf::Color(255, 0, 0));
	player2 = new Cycle(sf::Vector2f(105.f, 300.f), 0.f, sf::Color(0, 255, 0));
	player3 = new Cycle(sf::Vector2f(400.f, 5.f), 90.f, sf::Color(0, 0, 255));
	player4 = new Cycle(sf::Vector2f(400.f, 595.f), 270.f, sf::Color(255, 0, 255));

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
				delete player3;
				delete player4;
				player1 = new Cycle(sf::Vector2f(695.f, 300.f), 180.f, sf::Color(255, 0, 0));
				player2 = new Cycle(sf::Vector2f(105.f, 300.f), 0.f, sf::Color(0, 255, 0));
				player3 = new Cycle(sf::Vector2f(400.f, 5.f), 90.f, sf::Color(0, 0, 255));
				player4 = new Cycle(sf::Vector2f(400.f, 595.f), 270.f, sf::Color(255, 0, 255));
				paused = false;
			}
		}

		// TODO real controls
		if (sf::Joystick::isButtonPressed(0, 0))
		{
			player1->turn(90.f);
		}
		else if (sf::Joystick::isButtonPressed(0, 1))
		{
			player1->turn(0.f);
		}
		else if (sf::Joystick::isButtonPressed(0, 2))
		{
			player1->turn(180.f);
		}
		else if (sf::Joystick::isButtonPressed(0, 3))
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
		if (sf::Joystick::isButtonPressed(1, 0))
		{
			player3->turn(90.f);
		}
		else if (sf::Joystick::isButtonPressed(1, 1))
		{
			player3->turn(0.f);
		}
		else if (sf::Joystick::isButtonPressed(1, 2))
		{
			player3->turn(180.f);
		}
		else if (sf::Joystick::isButtonPressed(1, 3))
		{
			player3->turn(270.f);
		}
		if (sf::Joystick::isButtonPressed(2, 0))
		{
			player4->turn(90.f);
		}
		else if (sf::Joystick::isButtonPressed(2, 1))
		{
			player4->turn(0.f);
		}
		else if (sf::Joystick::isButtonPressed(2, 2))
		{
			player4->turn(180.f);
		}
		else if (sf::Joystick::isButtonPressed(2, 3))
		{
			player4->turn(270.f);
		}

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		// TODO oh my...
		player1->move(time);
		player2->move(time);
		player3->move(time);
		player4->move(time);
		player1->check_collision(*player2);
		player1->check_collision(*player3);
		player1->check_collision(*player4);
		player2->check_collision(*player1);
		player2->check_collision(*player3);
		player2->check_collision(*player4);
		player3->check_collision(*player1);
		player3->check_collision(*player2);
		player3->check_collision(*player4);
		player4->check_collision(*player1);
		player4->check_collision(*player2);
		player4->check_collision(*player3);

		player1->in(bounds);
		player2->in(bounds);
		player3->in(bounds);
		player4->in(bounds);

		// TODO generalize
		if (!paused)
		{
			win_s.str("");

			int living = 0;
			if (!player1->crashed) living++;
			if (!player2->crashed) living++;
			if (!player3->crashed) living++;
			if (!player4->crashed) living++;

			if (living == 0)
			{
				win_s << "DRAW";
				winner.setColor(sf::Color(255, 255, 255));
				paused = true;
			}
			else if (living == 1)
			{
				if (!player1->crashed)
				{
					winner.setColor(player1->color);
					player1->crashed = true;
				}
				if (!player2->crashed)
				{
					winner.setColor(player2->color);
					player2->crashed = true;
				}
				if (!player3->crashed)
				{
					winner.setColor(player3->color);
					player3->crashed = true;
				}
				if (!player4->crashed)
				{
					winner.setColor(player4->color);
					player4->crashed = true;
				}
				win_s << "WINNER!";
				paused = true;
			}
			winner.setString(win_s.str());
		}

		sf::FloatRect size = winner.getGlobalBounds();
		winner.setOrigin(size.width / 2, size.height / 2);

		fps_s.str("");
		fps_s << "FPS " << int (1.f / time);
		fps.setString(fps_s.str());

		window.clear(sf::Color(255, 255, 255));

		window.draw(bounds);

		player1->draw(window);
		player2->draw(window);
		player3->draw(window);
		player4->draw(window);

		window.draw(fps);
		window.draw(winner);

		window.display();
	}

	return EXIT_SUCCESS;
}
