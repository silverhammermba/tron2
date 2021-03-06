#include <iostream>
#include <array>
#include <list>
#include <sstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "cycle.hpp"
#include "helpers.hpp"

// big TODO list
// minor bugs:
// tail moves after colliding with it
// both players back up after a head-on collision
//
// features to add:
// display cause-of-death count for each player
// sub-controls for more players
// dynamically assign starting positions

using std::cin;
using std::cerr;
using std::endl;

sf::Font font;

int get_joystick(sf::Event& event);
bool input_is(sf::Event& event, sf::Keyboard::Key keycode, unsigned int joybutton);
void set_up(sf::RenderWindow& window, sf::View& view, sf::Text& winner, std::list<Cycle*>& player);

int main()
{
	font.loadFromFile("font.ttf");
	sf::Clock fclock; // frame fclock
	sf::Clock clock; // accumulative clock

	sf::Text fps {"", font, 12};
	fps.setPosition(5.f, 5.f);
	std::ostringstream fps_s;

	sf::Text winner {"", font, 24};
	std::ostringstream win_s;

	bool paused {true};

	sf::RenderWindow window
	{
		sf::VideoMode(800, 600, 32),
		"Light Cycles"
	};
	window.setVerticalSyncEnabled(false);
	window.setFramerateLimit(120);

	sf::View view (window.getView());

	sf::Event event;

	sf::RectangleShape bounds {v2f(790.f, 590.f)};
	bounds.setPosition(5.f, 5.f);
	bounds.setFillColor(sf::Color(30, 30, 30));

	std::list<Cycle*> player;

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

	float timescale = 1.f;

	set_up(window, view, winner, player);
	while (window.isOpen())
	{
		// process input
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
				set_up(window, view, winner, player);

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
							p->set_ready(true);
							taken = true;
							break;
						}
					}
					// if we can accomodate more players
					if (!taken && player.size() < 4)
					{
						// find color and starting position that aren't being used
						sf::Color c;
						for (auto& col : colors)
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
						player.push_back(new Cycle(s, startds[i], c, font, joystick));
						player.back()->set_text_pos(view.getCenter());
					}
				}
				// remove/unready players
				else if (input_is(event, sf::Keyboard::BackSpace, 6))
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
							unsigned int color_index;
							// find current color
							for (color_index = 0; color_index < colors.size(); color_index++)
								if (colors[color_index] == p->color)
									break;

							unsigned int current_color = color_index;
							color_index++;
							// find next available color
							for (; color_index % colors.size() != current_color; color_index++)
							{
								bool taken = false;
								for (auto q : player)
								{
									if (q->color == colors[color_index % colors.size()])
									{
										taken = true;
										break;
									}
								}
								if (!taken)
								{
									p->set_color(colors[color_index % colors.size()]);
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
				if (!(p->ready))
				{
					all_ready = false;
					break;
				}
			}
			if (all_ready && player.size() > 1)
			{
				paused = false;
				for (auto p : player)
					p->reset();
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

			win_s.str("");

			bool match_over = true;
			Cycle* survivor = nullptr;

			for(auto p : player)
				if (!(p->crashed))
				{
					if (survivor == nullptr)
						survivor = p;
					else
					{
						match_over = false;
						break;
					}
				}

			if (match_over)
			{
				if (survivor == nullptr)
				{
					win_s << "DRAW";
					winner.setColor(sf::Color(255, 255, 255));
				}
				else
				{
					win_s << "WINNER!";
					winner.setColor(survivor->color);
					survivor->scored();
				}

				paused = true;
				for (auto p : player)
					p->set_ready(false);
			}

			winner.setString(win_s.str());

			sf::FloatRect size = winner.getGlobalBounds();
			winner.setOrigin(size.width / 2, size.height / 2);
		}

		fps_s.str("");
		fps_s << "FPS " << int (1.f / time);
		fps.setString(fps_s.str());

		window.clear(sf::Color(255, 255, 255));

		window.draw(bounds);

		for (auto p : player)
			p->draw(window);
		for (auto p : player)
			p->draw_text(window, paused);

		window.draw(fps);
		window.draw(winner);

		window.display();
	}

	return EXIT_SUCCESS;
}

int get_joystick(sf::Event& event)
{
	if (event.type == sf::Event::JoystickButtonPressed)
		return event.joystickButton.joystickId;
	return -1;
}

bool input_is(sf::Event& event, sf::Keyboard::Key keycode, unsigned int joybutton)
{
	return (event.type == sf::Event::KeyPressed && event.key.code == keycode) || (event.type == sf::Event::JoystickButtonPressed && event.joystickButton.button == joybutton);
}

void set_up(sf::RenderWindow& window, sf::View& view, sf::Text& winner, std::list<Cycle*>& player)
{
	sf::Vector2u size = window.getSize();
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
	for (auto p : player)
		p->set_text_pos(view.getCenter());
	winner.setPosition(view.getCenter());
}
