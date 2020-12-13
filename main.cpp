#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <math.h>
#include <random>
#include <chrono>
#include <fstream>
#include <list>

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

sf::Font font;

double dt = 0.01;
int size_of_graph_x = 350;
int size_of_graph_y = 150;

int diagram_size=100;

int heating_turn = 0;
bool centre = false;
double Power_heating = 0.01;
double gravity = 0.00;

pair<double, double> force(double x, double y)
{
	pair<double, double> f;
	if (abs(x) + abs(y) >5)
	{
		double l, f0;
		l = sqrt(x * x + y * y);
		f0 = 20000./pow(l, 4)-1000000000./pow(l, 8);
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

	void calculate_force(const list<Ball*> &all_balls)
	{
		pair<double, double> flocal;
		f.first = 0; f.second = gravity;
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
		if (heating_turn!=0)
		{
			vx += vx / (vx * vx + vy * vy) * Power_heating * (double)heating_turn * dt;
			vy += vy / (vx * vx + vy * vy) * Power_heating * (double)heating_turn * dt;
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

	void centre_velocity(double Vx, double Vy)
	{
		vx -= Vx;
		vy -= Vy;
	}
};

class graph
{
	vector<pair<double, double>> data;
	list <Vertex*> all_vertex;
	sf::VertexArray id;
	sf::VertexArray axes;
	sf::Text textid;
	int x;
	int y;
	std::string name;

public:
	graph(pair<double, double> started_data, int number, std::string label)
	{
		x = 1000;
		y = 200 + number * 200;
		name = label;
		data.clear();
		data.push_back(started_data);
		id.setPrimitiveType(LinesStrip);
		id.clear();
		sf::Vertex* new_vertex = new sf::Vertex;
		new_vertex->position = sf::Vector2f(x + started_data.first, y + started_data.second);
		new_vertex->color = sf::Color(0, 0, 255);
		all_vertex.push_back(new_vertex);
		id.append(*new_vertex);

		axes.setPrimitiveType(LinesStrip);
		sf::Vertex v1, v2, v3;
		v1.position = sf::Vector2f(x + size_of_graph_x, y);
		v2.position = sf::Vector2f(x , y);
		v3.position = sf::Vector2f(x, y- size_of_graph_y);
		v1.color = sf::Color(0, 0, 0);
		v2.color = sf::Color(0, 0, 0);
		v3.color = sf::Color(0, 0, 0);
		axes.append(v1);
		axes.append(v2);
		axes.append(v3);

		textid.setPosition(sf::Vector2f(x, y));
		textid.setString(name);
		textid.setFont(font);
		textid.setCharacterSize(25);
		textid.setFillColor(sf::Color(0, 0, 0));
	}

	~graph()
	{
		for (auto v : all_vertex)delete v;
	}

	void show(sf::RenderWindow* window)
	{
		window->draw(id);
		window->draw(axes);
		window->draw(textid);
	}

	void push(pair<double, double> new_data)
	{
		data.push_back(new_data);
		sf::Vertex* new_vertex = new sf::Vertex;
		new_vertex->position = sf::Vector2f(x, y);
		new_vertex->color = sf::Color(0, 0, 255);
		all_vertex.push_back(new_vertex);
		id.append(*new_vertex);

		//рекомпановка графика
		double max_time = -10000000, min_time = 10000000, max_value = -10000000, min_value = 10000000;
		for (auto d : data)
		{
			if (d.first < min_time)min_time=d.first;
			if (d.first > max_time)max_time = d.first;
			if (d.second < min_value)min_value = d.second;
			if (d.second > max_value)max_value = d.second;
		}
		for (int i = 0; i < id.getVertexCount(); i++)
		{
			id[i].position = sf::Vector2f(x + (data[i].first-min_time)/(max_time-min_time)*size_of_graph_x, y - (data[i].second - min_value) / (max_value - min_value)*size_of_graph_y);
		}
		//конец рекомпановки
	}
};

class diagram
{
	vector<pair<double, double>> data;
	list <Vertex*> all_vertex;
	sf::VertexArray id;
	sf::VertexArray axes;
	sf::Text textid;

	int x;
	int y;
	int size;
	std::string name;

public:
	diagram(int local_size, int number, std::string label)
	{
		x = 1000;
		y = 200 + number * 200;
		name = label;
		data.clear();
		size = local_size;
		id.setPrimitiveType(LinesStrip);
		id.clear();
		for (int i = 0; i < size; i++)
		{
			sf::Vertex* new_vertex = new sf::Vertex;
			new_vertex->position = sf::Vector2f(x, y);
			new_vertex->color = sf::Color(0, 0, 255);
			all_vertex.push_back(new_vertex);
			id.append(*new_vertex);
		}
		
		axes.setPrimitiveType(LinesStrip);
		sf::Vertex v1, v2, v3;
		v1.position = sf::Vector2f(x + size_of_graph_x, y);
		v2.position = sf::Vector2f(x, y);
		v3.position = sf::Vector2f(x, y - size_of_graph_y);
		v1.color = sf::Color(0, 0, 0);
		v2.color = sf::Color(0, 0, 0);
		v3.color = sf::Color(0, 0, 0);
		axes.append(v1);
		axes.append(v2);
		axes.append(v3);

		textid.setPosition(sf::Vector2f(x, y));
		textid.setString(name);
		textid.setFont(font);
		textid.setCharacterSize(25);
		textid.setFillColor(sf::Color(0, 0, 0));
	}

	~diagram()
	{
		for (auto v : all_vertex)delete v;
	}

	void show(sf::RenderWindow* window)
	{
		window->draw(id);
		window->draw(axes);
		window->draw(textid);
	}

	void push(vector<pair<double, double>> new_data)
	{
		data.clear();
		for (auto d : new_data) data.push_back(d);

		//рекомпановка графика
		double max_time = -10000000, min_time = 10000000, max_value = -10000000, min_value = 10000000;
		for (auto d : data)
		{
			if (d.first < min_time)min_time = d.first;
			if (d.first > max_time)max_time = d.first;
			if (d.second < min_value)min_value = d.second;
			if (d.second > max_value)max_value = d.second;
		}
		for (int i = 0; i < id.getVertexCount(); i++)
		{
			id[i].position = sf::Vector2f(x + (data[i].first - min_time) / (max_time - min_time) * size_of_graph_x, y - (data[i].second - min_value) / (max_value - min_value) * size_of_graph_y);
		}
		//конец рекомпановки
	}
};

void cold(const list<Ball*> all_balls)
{
	for (auto b : all_balls)b->lose_energy();
}

void heat(const list<Ball*> all_balls)
{
	for (auto b : all_balls)b->rise_energy();
}

pair<double, double> count_energy(const list<Ball*>& all_balls, double current_time)
{
	pair<double, double> local_data;
	double E=0;
	for (auto b : all_balls)
	{
		E += b->vx * b->vx + b->vy * b->vy;
	}
	local_data.first = current_time;
	local_data.second = E;
	return local_data;
}

vector<pair<double, double>> count_bolzman(const list<Ball*>& all_balls)
{
	vector<pair<double, double>> local_data;
	pair<double, double> local_pair;
	for (int i = 0; i < diagram_size; i++)
	{
		local_pair.first = 1000 * i / diagram_size;
		local_pair.second = 0;
		for (auto b : all_balls)
		{
			if (b->y <= 1000 - 1000 * i / diagram_size && b->y > 1000 - 1000 * (i + 1) / diagram_size)local_pair.second += 1;
		}
		local_data.push_back(local_pair);
	}
	return local_data;
}

vector<pair<double, double>> count_makswell(const list<Ball*>& all_balls)
{
	double v;
	vector<pair<double, double>> local_data;
	pair<double, double> local_pair;
	for (double i = 0; i < diagram_size; i++)
	{
		local_pair.first = 10 * i / diagram_size;
		local_pair.second = 0;
		for (auto b : all_balls)
		{
			v = sqrt(b->vy * b->vy + b->vx + b->vx);
			if (v >= 10 * i / diagram_size && v < 10 * (i + 1) / diagram_size)local_pair.second += 1;
		}
		local_data.push_back(local_pair);
	}
	return local_data;
}

void centre_active(const list<Ball*>& all_balls)
{
	double Vx=0, Vy=0, N=0;
	for (auto b : all_balls)
	{
		N += 1;
		Vx += b->vx;
		Vy += b->vy;
	}
	Vx = Vx / N;
	Vy = Vy / N;
	for (auto b : all_balls)
	{
		b->centre_velocity(Vx, Vy);
	}
}

int main()
{
	list<Ball*> all_balls;
	list<graph*> all_graphs;
	list<diagram*> all_diagrams;
	const int Number_balls = 110, fps = 10;
	int L=sqrt(Number_balls), j;
	double current_time = 0;
	int shag = 0;
	for (int i=0; i<Number_balls; i++)
	{
		Ball* ball = new Ball;
		all_balls.push_back(ball);
	}
	j = 0;
	for (auto i : all_balls)
	{
		if (j < 100)
		{
			i->x = j % L * 15 + 400;
			i->y = (j - j % L) * 15 / L + 700;
			j++;
		}
		else
		{
			i->x = j % 3 * 15 + 460;
			i->y = (j - j % 3) * 15 / 3 - 200;
			j++;
		}

	}
	pair<double, double> local_data;
	local_data.first = 0; local_data.second = 0;
	graph gr1(local_data, 0, "Temperature");
	all_graphs.push_back(&gr1);
	graph gr2(local_data, 1, "Un-ready");
	all_graphs.push_back(&gr2);
	diagram d1(diagram_size, 3, "Bolzman");
	all_diagrams.push_back(&d1);
	diagram d2(diagram_size, 2, "Makswell");
	all_diagrams.push_back(&d2);

	RenderWindow window(VideoMode(1400, 1000), "My project!");
	sf::RectangleShape fon;
	fon.setFillColor(Color(150, 150, 255));
	fon.setSize(sf::Vector2f(800, 800));
	fon.setPosition(sf::Vector2f(100, 100));
	font.loadFromFile("CyrilicOld.ttf");
	sf::Text tx1;
	tx1.setPosition(sf::Vector2f(0,0));
	tx1.setString("Управление: клавиши-стрелки вверх и вниз - нагреть и охладить систему.\n H и C - соответственно включить адиабатический нагрев и охлаждение\n T - отключить гравитацию, G - включить/сменить направление гравитации.");
	tx1.setFont(font);
	tx1.setCharacterSize(30);
	tx1.setFillColor(sf::Color(0, 0, 0));

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Down)cold(all_balls);
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::Up)heat(all_balls);
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::H)if (heating_turn < 1)heating_turn +=1;
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::C)if (heating_turn > -1)heating_turn -= 1;
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::N) { if (centre == false)centre = true; else centre = false; }
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::G)if (gravity == -0.01)gravity = 0.01;else gravity = -0.01;
			if (event.type == Event::KeyPressed && event.key.code == sf::Keyboard::T)gravity=0;
			if (event.type == Event::Closed)
				window.close();
		}
		window.clear(Color(200, 200, 200, 0));
		window.draw(fon);
		window.draw(tx1);
		for (int k = 0; k < fps; k++)
		{
			current_time += dt;
			if(centre)centre_active(all_balls);
			for (auto i : all_balls)
			{
				i->calculate_force(all_balls);
			}
			for (auto i : all_balls)
			{
				i->live();
			}
		}
		for (auto i : all_balls)i->show(&window);
		if (shag%100==0)
		{
			local_data = count_energy(all_balls, current_time);
			gr1.push(local_data);
			d1.push(count_bolzman(all_balls));
			d2.push(count_makswell(all_balls));
		}
		for (auto i : all_graphs)i->show(&window);
		for (auto i : all_diagrams)i->show(&window);
		//sf::sleep(sf::milliseconds(2000));
		window.display();
		shag++;
	}
	for (auto b : all_balls)delete b;
	return 0;
}

