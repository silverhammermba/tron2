#include <iostream>
#include <array>
#include <list>
#include <sstream>
#include <cmath>
#include <SFML/Audio/Music.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "cycle.hpp"
#include "helpers.hpp"

#define _USE_MATH_DEFINES

using std::cin;
using std::cerr;
using std::endl;

void set_volume(Cycle *p1, Cycle *p2, sf::Music *track);
int get_joystick(sf::Event & event);
bool input_is (sf::Event & event, int keycode, int joybutton);

int main(int argc, char *argv[])
{
	sf::Clock fclock; // frame fclock
	sf::Clock clock; // accumulative clock

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

	bool paused {true};

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

	// TODO dynamically assign these?
	std::array<v2f, 4> starts
	{
		v2f(105.f, 300.f),
		v2f(695.f, 300.f),
		v2f(400.f, 5.f),
		v2f(400.f, 595.f)
	};

	std::array<float, 4> startds
	{
		0.f,
		180.f,
		90.f,
		270.f
	};

	std::array<sf::Color, 10> colors
	{
		sf::Color(255, 0, 0),
		sf::Color(0, 255, 0),
		sf::Color(0, 0, 255),
		sf::Color(255, 0, 255),
		sf::Color(127, 0, 255),
		sf::Color(255, 255, 0),
		sf::Color(230, 127, 0),
		sf::Color(0, 255, 255),
		sf::Color(0, 100, 0),
		sf::Color(150, 150, 150)
	};

	// (shitty) MUSIC!!!
	/*
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

	bass.setVolume(0.f);
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
	*/

	float timescale = 1.f;

	while (window.isOpen())
	{
		// process input
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
				for (auto p : player) p->set_text_pos(view.getCenter());
				winner.setPosition(view.getCenter());
			}
			// quit
			if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)
				window.close();
			if (paused)
			{
				// add players
				if (input_is(event, sf::Keyboard::Return, 7))
				{
					int joystick = get_joystick(event);

					// check if the controller is taken
					bool taken = false;
					for (auto p : player)
					{
						if (p->joystick == joystick)
						{
							//cerr << "Player taken, readying\n";
							p->set_ready(true);
							taken = true;
							break;
						}
					}
					// if we can accomodate more players
					if (player.size() < 4)
					{
						if (!taken)
						{
							//cerr << "Adding new player\n";
							// find color and starting position that aren't being used
							sf::Color c;
							for (auto col : colors)
							{
								bool taken = false;
								for (auto p : player)
								{
									if (p->color == col)
									{
										taken = true;
										break;
									}
								}
								if (!taken)
								{
									c = col;
									break;
								}
							}
							v2f s;
							int i = 0; // TODO HACKY AS BALLS
							for (auto strt : starts)
							{
								bool taken = false;
								for (auto p : player)
								{
									if (p->start == strt)
									{
										taken = true;
										break;
									}
								}
								if (!taken)
								{
									s = strt;
									break;
								}
								i++;
							}
							player.push_back(new Cycle(s, startds[i], c, joystick));
							player.back()->set_text_pos(view.getCenter());
						}
					}
				}
				// remove/unready players
				else if (input_is(event, sf::Keyboard::Back, 6))
				{
					int joystick = get_joystick(event);

					for (auto p : player)
					{
						if (p->joystick == joystick)
						{
							if (p->ready)
								p->set_ready(false);
							else
								player.remove(p);
							break;
						}
					}
				}
				// change colors
				else if (input_is(event, sf::Keyboard::C, 5))
				{
					int joystick = get_joystick(event);

					for (auto p : player)
					{
						if (p->joystick == joystick)
						{
							int i;
							int l = colors.size();
							// find current color
							for (i = 0; i < l; i++)
							{
								if (colors[i] == p->color) break;
							}
							int col = i;
							i++;
							// find next available color
							for (; i % l != col; i++)
							{
								bool taken = false;
								for (auto q : player)
								{
									if (q->color == colors[i % l])
									{
										taken = true;
										break;
									}
								}
								if (!taken)
								{
									p->set_color(colors[i % l]);
									break;
								}
							}
							break;
						}
					}
				}
			}
			else
			{
				for (auto p : player)
					p->bind(event);
			}
		}

		float time = fclock.getElapsedTime().asSeconds();
		fclock.restart();

		// slow motion start
		float stime = clock.getElapsedTime().asSeconds();
		timescale = std::min(3.f, stime) / 3.f;

		if (paused)
		{
			bool all_ready {true};
			for (auto p : player)
			{
				if (!p->ready)
				{
					all_ready = false;
					break;
				}
			}
			if (all_ready)
			{
				paused = false;
				for (auto p : player) p->reset();
				clock.restart();
			}
		}
		else
		{
			for (auto p : player)
				p->move(time * timescale);
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

			// TODO visual bug when players collide head-on
			for (auto p : player)
				p->backup();

			/* adjust track volume */
			// TODO better way? need to allocate music better
			/*
			int i = 0;
			for (auto it = player.begin(); it != player.end(); it++)
			{
				int j = 0;
				for (auto jt = player.begin(); jt != player.end(); jt++)
				{
					if (i == 0 && j == 1)
						set_volume(*it, *jt, &pian);
					else if (i == 0 && j == 2)
						set_volume(*it, *jt, &elgt);
					else if (i == 0 && j == 3)
						set_volume(*it, *jt, &psyn);
					else if (i == 1 && j == 2)
						set_volume(*it, *jt, &rhds);
					else if (i == 1 && j == 3)
						set_volume(*it, *jt, &synl);
					else if (i == 2 && j == 3)
						set_volume(*it, *jt, &synp);
					j++;
				}
				i++;
			}
			*/

			win_s.str("");

			int living = 0;
			for(auto p : player)
				if (!p->crashed) living++;

			if (living == 0)
			{
				win_s << "DRAW";
				winner.setColor(sf::Color(255, 255, 255));
				paused = true;
				for (auto p : player) p->set_ready(false);
			}
			else if (living == 1)
			{
				for (auto p : player)
				{
					if (!p->crashed)
					{
						winner.setColor(p->color);
						p->scored();
						//p->crashed = true;
						break;
					}
				}
				win_s << "WINNER!";
				paused = true;
				for (auto p : player) p->set_ready(false);
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

			sf::FloatRect size = winner.getGlobalBounds();
			winner.setOrigin(size.width / 2, size.height / 2);
		}

		fps_s.str("");
		fps_s << "FPS " << int (1.f / time);
		fps.setString(fps_s.str());

		vol_s.str("");
		/*
		vol_s << "Pian " << int (pian.getVolume()) << endl
		      << "Elec " << int (elgt.getVolume()) << endl
			  << "PanS " << int (psyn.getVolume()) << endl
			  << "Rhod " << int (rhds.getVolume()) << endl
			  << "Lead " << int (synl.getVolume()) << endl
			  << "SPad " << int (synp.getVolume());
		*/
		vol.setString(vol_s.str());

		window.clear(sf::Color(255, 255, 255));

		window.draw(bounds);

		for (auto p : player)
			p->draw(window, paused);

		window.draw(fps);
		window.draw(vol);
		window.draw(winner);

		window.display();
	}

	return EXIT_SUCCESS;
}

void set_volume(Cycle *p1, Cycle *p2, sf::Music *track)
{
	float vol = 100.f - clamp<float>(0.f, v2dist<float>(p1->get_edge().getPosition(), p2->get_edge().getPosition()), 300.f) / 3.f;
	track->setVolume(vol);
}

int get_joystick(sf::Event & event)
{
	if (event.type == sf::Event::KeyPressed)
		return -1;
	else if (event.type == sf::Event::JoystickButtonPressed)
		return event.joystickButton.joystickId;
}

bool input_is (sf::Event & event, int keycode, int joybutton)
{
	return (event.type == sf::Event::KeyPressed && event.key.code == keycode) || (event.type == sf::Event::JoystickButtonPressed && event.joystickButton.button == joybutton);
}
