#include <iostream>
#include <list>
#include <sstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio/Music.hpp>
#include "cycle.hpp"
#include "helpers.hpp"

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
	fps.setPosition(5.f, 5.f);
	std::ostringstream fps_s;

	sf::Text vol;
	vol.setCharacterSize(12);
	vol.setPosition(20.f, 50.f);
	std::ostringstream vol_s;

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
	window.setVerticalSyncEnabled(false);

	sf::View view (window.getView());

	sf::Event event;

	sf::RectangleShape bounds {v2f(790.f, 590.f)};
	bounds.setPosition(5.f, 5.f);
	bounds.setFillColor(sf::Color(30, 30, 30));

	std::list<Cycle *> player;

	player.push_back(new Cycle(v2f(695.f, 300.f), 180.f, sf::Color(255, 0, 0)));
	player.push_back(new Cycle(v2f(105.f, 300.f), 0.f, sf::Color(0, 255, 0)));
	if (PLAYERS > 2)
	{
		player.push_back(new Cycle(v2f(400.f, 5.f), 90.f, sf::Color(0, 0, 255)));
		if (PLAYERS > 3)
			player.push_back(new Cycle(v2f(400.f, 595.f), 270.f, sf::Color(255, 0, 255)));
	}

	// (shitty) MUSIC!!!
	sf::Music bass;
	sf::Music pian;
	sf::Music elgt;
	sf::Music psyn;
	sf::Music rhds;
	sf::Music synl;
	sf::Music synp;

	// TODO error check
	bass.openFromFile("loops/Bass.wav");
	pian.openFromFile("loops/Piano.wav");
	elgt.openFromFile("loops/Electric_Guitar.wav");
	psyn.openFromFile("loops/Panning_Synth.wav");
	rhds.openFromFile("loops/Rhodes.wav");
	synl.openFromFile("loops/Solo_Synth_Lead.wav");
	synp.openFromFile("loops/Synth_Pad.wav");

	bass.setLoop(true);
	pian.setLoop(true);
	elgt.setLoop(true);
	psyn.setLoop(true);
	rhds.setLoop(true);
	synl.setLoop(true);
	synp.setLoop(true);

	bass.setVolume(100.f);
	pian.setVolume(0.f);
	elgt.setVolume(0.f);
	psyn.setVolume(0.f);
	rhds.setVolume(0.f);
	synl.setVolume(0.f);
	synp.setVolume(0.f);

	bass.play();
	pian.play();
	elgt.play();
	psyn.play();
	rhds.play();
	synl.play();
	synp.play();

	sf::Music *track[9];
	// TODO reorder?
	track[0] = &elgt;
	// 3 player
	track[1] = &synp;
	track[4] = &psyn;
	// 4 player
	track[2] = &rhds;
	track[5] = &synl;
	track[8] = &pian;

	bool menu = true;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
			{
				sf::Vector2u size = window.getSize();
				float prop = float (size.x) / size.y;
				if (size.x * 3 < size.y * 4)
				{
					view.setSize(v2f(800, (800.f * size.y) / size.x));
					window.setView(view);
				}
				else if (size.x * 3 > size.y * 4)
				{
					view.setSize(v2f((600.f * size.x) / size.y, 600.f));
					window.setView(view);
				}
				winner.setPosition(view.getCenter());
			}
			if (menu)
			{
				if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)
					window.close();
				else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Back)
					player.pop_back();
				else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space)
					menu = false;
			}
			else
			{
				if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)
				{
					menu = true;
					for (auto p : player)
						p->reset();

					paused = false;
					pian.setVolume(0.f);
					elgt.setVolume(0.f);
					psyn.setVolume(0.f);
					rhds.setVolume(0.f);
					synl.setVolume(0.f);
					synp.setVolume(0.f);
				}
				else if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::R))
				{
					for (auto p : player)
						p->reset();

					paused = false;
					pian.setVolume(0.f);
					elgt.setVolume(0.f);
					psyn.setVolume(0.f);
					rhds.setVolume(0.f);
					synl.setVolume(0.f);
					synp.setVolume(0.f);
				}
				else
				{
					for (auto p : player)
						p->bind(event);
				}
			}
		}

		float time = clock.getElapsedTime().asSeconds();
		clock.restart();

		if (menu)
		{
		}
		else
		{
			for (auto p : player)
				p->move(time);
			// TODO better way?
			for (auto p1 : player)
			{
				for (auto p2 : player)
				{
					if (p1 != p2)
					{
						p1->check_collision(*p2);
						p2->check_collision(*p1);
					}
				}
			}
			for (auto p : player)
				p->in(bounds);

			/* adjust track volume */
			// TODO better way? need to allocate music better
			/*
			int i = 0;
			int j;
			for (auto p1 : player)
			{
				j = 0;
				for (auto p2 : player)
				{
					if (p1 != p2)
					{
						float vol = 100.f - clamp<float>(0.f, v2dist<float>(p1->get_edge().getPosition(), p2->get_edge().getPosition()), 300.f) / 3.f;
						track[3 * i + j - 1]->setVolume(vol);
						if (!paused)
						cerr << "Vol " << i << "," << j << "=" << vol << endl;
					}
					j++;
				}
				i++;
			}
			*/

			// TODO generalize
			if (!paused)
			{
				win_s.str("");

				int living = 0;
				for(auto p : player)
					if (!p->crashed) living++;

				if (living == 0)
				{
					win_s << "DRAW";
					winner.setColor(sf::Color(255, 255, 255));
					paused = true;
				}
				else if (living == 1)
				{
					for (auto p : player)
					{
						if (!p->crashed)
						{
							winner.setColor(p->color);
							//p->crashed = true;
							break;
						}
					}
					win_s << "WINNER!";
					paused = true;
				}
				/*
				if (paused)
				{
					pian.setVolume(100.f);
					elgt.setVolume(100.f);
					psyn.setVolume(100.f);
					rhds.setVolume(100.f);
					synl.setVolume(100.f);
					synp.setVolume(100.f);
				}
				*/
				winner.setString(win_s.str());
			}

			sf::FloatRect size = winner.getGlobalBounds();
			winner.setOrigin(size.width / 2, size.height / 2);
		}

		fps_s.str("");
		fps_s << "FPS " << int (1.f / time);
		fps.setString(fps_s.str());

		vol_s.str("");
		vol_s << "Pian " << int (pian.getVolume()) << endl
		      << "Elec " << int (elgt.getVolume()) << endl
			  << "PanS " << int (psyn.getVolume()) << endl
			  << "Rhod " << int (rhds.getVolume()) << endl
			  << "Lead " << int (synl.getVolume()) << endl
			  << "SPad " << int (synp.getVolume());
		vol.setString(vol_s.str());

		window.clear(sf::Color(255, 255, 255));

		window.draw(bounds);

		for (auto p : player)
			p->draw(window);

		window.draw(fps);
		window.draw(vol);
		window.draw(winner);

		window.display();
	}

	return EXIT_SUCCESS;
}
