#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

using namespace sf;
using namespace std;

int dt = 0.001;

pair<double, double> force(int x)
{	
	pair<double, double> f;
	f.first = 1;
	f.second = 1;
	return f;
}

class Ball
{
public:
	double x;
	double y;
	double vx;
	double vy;
	CircleShape id; 
	Ball()
	{
		x = 0;
		y = 0;
		vx = 0;
		vy = 0;
		id.setRadius(4);
		
		id.setFillColor(Color(0, 255, 0));
	}

	void show(sf::RenderWindow* window)
	{
		sf::Vector2f k;
		k.x = (int)x;
		k.y = (int)y;
		id.setPosition(k);
		window->draw(id);
	}

	void live(const vector<Ball> &all_balls)
	{
		x += vx;
		y += vy;
	}
};

int main()
{
	vector<Ball> all_balls;
	const int Number_balls = 10100, fps=1000;
	for (int i = 0; i < Number_balls; i++) 
	{ 
		Ball* ball = new Ball; 
		all_balls.push_back(*ball); 
	}
	for (int i = 0; i < Number_balls; i++) 
	{ 
		if (i < 10000)
		{
			all_balls[i].x = (i - i % 100) / 20 + 200;
			all_balls[i].y = i % 100 * 5 + 300;
		}
		else
		{
			all_balls[i].x = (i-10000 - i % 10) / 2 + 900;
			all_balls[i].y = i % 10 * 5 + 500;
			all_balls[i].vx = -0.01;
		}
	}

	RenderWindow window(VideoMode(1400, 1000), "My project!");

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}
		window.clear(Color(150, 150, 255, 0));
		for (int k = 0; k < fps; k++)for (int i = 0; i < Number_balls; i++) { all_balls[i].live(all_balls); }
		for (int i = 0; i < Number_balls; i++) { all_balls[i].show(&window); }
		
		window.display();
	}

	return 0;
}