#include <iostream>
#include <sstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "cycle.hpp"

#define _USE_MATH_DEFINES

using std::cin;
using std::cerr;
using std::endl;

int main(int argc, char *argv[])
{
	int PLAYERS = atoi(argv[1]);

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
	//window.setVerticalSyncEnabled(true);

	sf::View view (window.getView());

	sf::Event event;

	sf::RectangleShape bounds {sf::Vector2f(790.f, 590.f)};
	bounds.setPosition(5.f, 5.f);
	bounds.setFillColor(sf::Color(30, 30, 30));

	Cycle *player[PLAYERS];

	player[0] = new Cycle(sf::Vector2f(695.f, 300.f), 180.f, sf::Color(255, 0, 0));
	player[1] = new Cycle(sf::Vector2f(105.f, 300.f), 0.f, sf::Color(0, 255, 0));
	if (PLAYERS > 2)
	{
		player[2] = new Cycle(sf::Vector2f(400.f, 5.f), 90.f, sf::Color(0, 0, 255));
		if (PLAYERS > 3)
			player[3] = new Cycle(sf::Vector2f(400.f, 595.f), 270.f, sf::Color(255, 0, 255));
	}

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed ||
			  ((event.type == sf::Event::KeyReleased) && (event.key.code == sf::Keyboard::Escape)))
				window.close();
			else if (event.type == sf::Event::Resized)
			{
				sf::Vector2u size = window.getSize();
				float prop = float (size.x) / size.y;
				if (size.x * 3 < size.y * 4)
				{
					view.setSize(sf::Vector2f(800, (800.f * size.y) / size.x));
					window.setView(view);
				}
				else if (size.x * 3 > size.y * 4)
				{
					view.setSize(sf::Vector2f((600.f * size.x) / size.y, 600.f));
					window.setView(view);
				}
				/*
				view.setSize((sf::Vector2f)window.getSize());
				window.setView(view);
				fps.setPosition(view.getCenter() - view.getSize() / 2.f);
				*/
				// TODO center
				winner.setPosition(view.getCenter());
			}
			else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::R))
			{
				for (int i = 0; i < PLAYERS; i++)
					delete player[i];
				player[0] = new Cycle(sf::Vector2f(695.f, 300.f), 180.f, sf::Color(255, 0, 0));
				player[1] = new Cycle(sf::Vector2f(105.f, 300.f), 0.f, sf::Color(0, 255, 0));
				if (PLAYERS > 2)
				{
					player[2] = new Cycle(sf::Vector2f(400.f, 5.f), 90.f, sf::Color(0, 0, 255));
					if (PLAYERS > 3)
						player[3] = new Cycle(sf::Vector2f(400.f, 595.f), 270.f, sf::Color(255, 0, 255));
				}
				paused = false;
			}
		}

		// TODO real controls
		for (int i = 0; i < PLAYERS && i < 2; i++)
		{
			if (sf::Joystick::isButtonPressed(i, 0))
			{
				player[i]->turn(90.f);
			}
			else if (sf::Joystick::isButtonPressed(i, 1))
			{
				player[i]->turn(0.f);
			}
			else if (sf::Joystick::isButtonPressed(i, 2))
			{
				player[i]->turn(180.f);
			}
			else if (sf::Joystick::isButtonPressed(i, 3))
			{
				player[i]->turn(270.f);
			}
		}
		if (PLAYERS > 2)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				player[2]->turn(0.f);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			{
				player[2]->turn(90.f);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				player[2]->turn(180.f);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			{
				player[2]->turn(270.f);
			}
		}
		if (PLAYERS > 3)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			{
				player[3]->turn(0.f);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			{
				player[3]->turn(90.f);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
			{
				player[3]->turn(180.f);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
			{
				player[3]->turn(270.f);
			}
		}

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		for (int i = 0; i < PLAYERS; i++)
			player[i]->move(time);
		for (int i = 0; i < PLAYERS; i++)
		{
			for (int j = i; j < PLAYERS; j++)
			{
				player[i]->check_collision(*player[j]);
				player[j]->check_collision(*player[i]);
			}
		}
		for (int i = 0; i < PLAYERS; i++)
			player[i]->in(bounds);

		// TODO generalize
		if (!paused)
		{
			win_s.str("");

			int living = 0;
			for (int i = 0; i < PLAYERS; i++)
				if (!player[i]->crashed) living++;

			if (living == 0)
			{
				win_s << "DRAW";
				winner.setColor(sf::Color(255, 255, 255));
				paused = true;
			}
			else if (living == 1)
			{
				for (int i = 0; i < PLAYERS; i++)
				{
					if (!player[i]->crashed)
					{
						winner.setColor(player[i]->color);
						player[i]->crashed = true;
						break;
					}
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

		for (int i = 0; i < PLAYERS; i++)
			player[i]->draw(window);

		window.draw(fps);
		window.draw(winner);

		window.display();
	}

	return EXIT_SUCCESS;
}
