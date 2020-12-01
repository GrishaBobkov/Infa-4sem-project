#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <math.h>
#include <random>
#include <chrono>
#include <fstream>

using namespace sf;
using namespace std;

double get_time()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count() / 1e6;
}

int rand_uns(int min, int max)
{
	unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
	static std::default_random_engine e(seed);
	std::uniform_int_distribution<int> d(min, max);
	return d(e);
}

double dt = 0.01;
bool heating_turn = false;
double Power_heating = 0.001;

pair<double, double> force(double x, double y)
{
	pair<double, double> f;
	if (abs(x) + abs(y) >5)
	{
		double l, f0;
		l = sqrt(x * x + y * y);
		f0 = 20000/pow(l, 4)-1000000000/pow(l, 8);
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
		vx = rand_uns(-1000, 1000);
		vy = rand_uns(-1000, 1000);
		vx = vx / 1000;
		vy = vy / 1000;
		f.first = 0;
		f.second = 0;
		id.setRadius(4);
		id.setFillColor(Color(0, 255, 0));
	}

	void show(sf::RenderWindow* window)
	{
		int color;
		color = (int)(sqrt(vx * vx + vy * vy)*50);
		if (color > 255)color = 255;
		id.setFillColor(Color(color, 0, 255- color));
		sf::Vector2f k;
		k.x = (int)x;
		k.y = (int)y;
		id.setPosition(k);
		window->draw(id);
	}

	void calculate_force(const vector<Ball*> &all_balls)
	{
		pair<double, double> flocal;
		f.first = 0; f.second = 0.001;
		for (auto b : all_balls)
		{
			flocal = force(b->x - x, b->y - y);
			f.first += flocal.first;
			f.second += flocal.second;
		}
		if (x < 100)f.first += (100 - x)/10;
		if (x > 900)f.first -= (x - 900) / 10;
		if (y < 100)f.second += (100 - y) / 10;
		if (y > 900)f.second -= (y - 900) / 10;
	}

	void live()
	{
		if (heating_turn)
		{
			vx += vx / (vx * vx + vy * vy) * Power_heating * dt;
			vy += vy / (vx * vx + vy * vy) * Power_heating * dt;
		}
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
		if (abs(vx) < 3)vx = vx * 1.1;
		if (abs(vy) < 3)vy = vy * 1.1;
	}
};

class graph
{
	vector<pair<double, double>> data;
	int x;
	int y;
public:
	graph(vector<pair<double, double>> &d)
	{

	}
	~graph()
	{

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
	const int Number_balls = 25, fps = 10;
	int L=sqrt(Number_balls);
	for (int i = 0; i < Number_balls; i++)
	{
		Ball* ball = new Ball;
		all_balls.push_back(ball);
	}
	for (int i = 0; i < Number_balls; i++)
	{
		all_balls[i]->x = (i - i % L) * 15 / L + 400;
		all_balls[i]->y = i % L * 15 + 400;
	}

	RenderWindow window(VideoMode(1400, 1000), "My project!");
	sf::RectangleShape fon;
	fon.setFillColor(Color(150, 150, 255));
	sf::Vector2f k;
	k.x = 800;
	k.y = 800;
	fon.setSize(k);
	k.x = 100;
	k.y = 100;
	fon.setPosition(k);
	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Down)cold(all_balls);
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Up)heat(all_balls);
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::H)heating_turn = true;
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::C)heating_turn = false;
			if (event.type == Event::Closed)
				window.close();
		}
		window.clear(Color(100, 100, 200, 0));
		window.draw(fon);
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
