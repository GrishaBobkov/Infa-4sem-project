#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <set>
#include <math.h>

using namespace sf;
using namespace std;

double dt = 0.001;//Единица времени - 1мкс
double dx = 0.01;//Еденица длины - 1мм
double Lx = 1000;
double Ly = 1000;
int smp = 100;

pair<double, double> force(double x, double y)
{	
	pair<double, double> f;
	double l, f0;
	l = sqrt(x * x + y + y);
	f0 = 0,00015/dx*(pow(2, (dx-l) / (0.01 * dx))-1);
	if (l < 1.01 * dx)
	{
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
	int n;
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

	void choose_place(vector<vector<vector<Ball*>>>& the_nearest_balls)
	{
		int xlocal, ylocal;
		xlocal = x * smp / Lx;
		ylocal = y * smp / Ly;
		xlocal = xlocal % smp;
		ylocal = ylocal % smp;
		//the_nearest_balls[xlocal][ylocal].push_back(this);
	}

	void calculate_force(const vector<vector<vector<Ball*>>>& the_nearest_balls)
	{
		int xlocal, ylocal;
		xlocal = x * smp / Lx;
		ylocal = y * smp / Ly;
		pair<double, double> flocal;
		f.first = 0; f.second = 0;
		/*for(int i=-1; i<2; i++)for (int j = -1; j < 2; j++)
		for (auto b : the_nearest_balls[(xlocal+i)%smp][(ylocal+j)%smp])
		{
			flocal = force(b->x - x, b->y - y);
			f.first += flocal.first;
			f.second += flocal.second;
		}*/
	}

	void live()
	{
		vx += f.first * dt;
		vy += f.second * dt;
		x += vx * dt;
		y += vy * dt;
	}
};

void empty_the_nearest_balls(const vector<vector<vector<Ball*>>>& the_nearest_balls)
{
	for (auto i : the_nearest_balls)for (auto j : i)j.clear();
}

int main()
{
	vector<Ball*> all_balls;
	vector<vector<vector<Ball*>>> the_nearest_balls;
	the_nearest_balls.resize(smp);
	for (auto i : the_nearest_balls)i.resize(smp);
	const int Number_balls = 10100, fps=1000;
	for (int i = 0; i < Number_balls; i++) 
	{ 
		Ball* ball = new Ball; 
		all_balls.push_back(ball); 
	}
	for (int i = 0; i < Number_balls; i++) 
	{ 
		if (i < 10000)
		{
			all_balls[i]->x = (i - i % 100) / 20 + 200;
			all_balls[i]->y = i % 100 * 5 + 300;
		}
		else
		{
			all_balls[i]->x = (i-10000 - i % 10) / 2 + 900;
			all_balls[i]->y = i % 10 * 5 + 500;
			all_balls[i]->vx = -10;
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
		for (int k = 0; k < fps; k++)
		{
			empty_the_nearest_balls(the_nearest_balls);
			for (int i = 0; i < Number_balls; i++)
			{
				all_balls[i]->choose_place(the_nearest_balls);
			}
			for (int i = 0; i < Number_balls; i++)
			{
				all_balls[i]->calculate_force(the_nearest_balls);
			}
			for (int i = 0; i < Number_balls; i++)
			{
				all_balls[i]->live();
			}
		}
		for (int i = 0; i < Number_balls; i++) { all_balls[i]->show(&window); }
		
		window.display();
	}
	for (auto b : all_balls)delete b;
	return 0;
}