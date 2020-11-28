#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <math.h>

using namespace sf;
using namespace std;

double dt = 0.01;

pair<double, double> force(double x, double y)
{
	pair<double, double> f;
	if (abs(x) + abs(y) < 100)
	{
		double l, f0;
		l = sqrt(x * x + y * y);
		f0 = 100/pow(l, 4)-60000/pow(l, 8);
		if (x != 0)f.first = f0 * x / l;
		else f.first = 0;
		if (y != 0)f.second = f0 * y / l;
		else f.second = 0;
	}
	else
	{
		f.first = 0;
		f.second = 0;
	}
	return f;
}

class Ball
{
public:
	double x;
	double y;
	double vx;
	double vy;
	pair <double, double> f;
	CircleShape id;
	Ball()
	{
		x = 0;
		y = 0;
		vx = 0;
		vy = 0;
		f.first = 0;
		f.second = 0;
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

	void calculate_force(const vector<Ball*> &all_balls)
	{
		pair<double, double> flocal;
		f.first = 0; f.second = 0;
		for (auto b : all_balls)
		{
			flocal = force(b->x - x, b->y - y);
			f.first += flocal.first;
			f.second += flocal.second;
		}
		if (x < 200)f.first += dt * (200 - x) / 10;
		if (x > 800)f.first -= dt * (x - 800) / 10;
		if (y < 200)f.second += dt * (200 - y) / 10;
		if (y > 800)f.second -= dt * (y - 800) / 10;

	}

	void live()
	{
		vx += f.first * dt;
		vy += f.second * dt;
		x += vx * dt;
		y += vy * dt;
	}

	void lose_energy()
	{
		vx = vx * 0.9;
		vy = vy * 0.9;
	}

	void rise_energy()
	{
		vx = vx * 1.1;
		vy = vy * 1.1;
	}
};

void cold(const vector<Ball*> all_balls)
{
	for (auto b : all_balls)b->lose_energy();
}

void heat(const vector<Ball*> all_balls)
{
	for (auto b : all_balls)b->rise_energy();
}

int main()
{
	vector<Ball*> all_balls;
	const int Number_balls = 100, fps = 10;
	int L=sqrt(Number_balls);
	for (int i = 0; i < Number_balls; i++)
	{
		Ball* ball = new Ball;
		all_balls.push_back(ball);
	}
	for (int i = 0; i < Number_balls; i++)
	{
		all_balls[i]->x = (i - i % L) * 10 / L + 400;
		all_balls[i]->y = i % L * 10 + 400;
	}

	RenderWindow window(VideoMode(1400, 1000), "My project!");

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Down)cold(all_balls);
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Up)heat(all_balls);
			if (event.type == Event::Closed)
				window.close();
		}
		window.clear(Color(150, 150, 255, 0));
		for (int k = 0; k < fps; k++)
		{
			for (int i = 0; i < Number_balls; i++)
			{
				all_balls[i]->calculate_force(all_balls);
			}
			for (int i = 0; i < Number_balls; i++)
			{
				all_balls[i]->live();
			}
		}
		for (int i = 0; i < Number_balls; i++) { all_balls[i]->show(&window); }
		//sf::sleep(sf::milliseconds(2000));
		window.display();
	}
	for (auto b : all_balls)delete b;
	return 0;
}