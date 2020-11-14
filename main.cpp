#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>

using namespace sf;
using namespace std;

class Ball
{
public:
	double x;
	double y;
	CircleShape id; 
	Ball()
	{
		x = 0;
		y = 0;
		id.setRadius(5);
		
		id.setFillColor(Color(0xFF, 0x0, 0x0));
	}

	void show(sf::RenderWindow* window)
	{
		sf::Vector2f k;
		k.x = (int)x;
		k.y = (int)y;
		id.setPosition(k);
		window->draw(id);
	}
};

int main()
{
	vector<Ball> all_balls;
	for (int i = 0; i < 10; i++) { Ball* ball = new Ball; all_balls.push_back(*ball); }
	for (int i = 0; i < 10; i++) { all_balls[i].x = i * 50 + 100; }

	// Объект, который, собственно, является главным окном приложения
	RenderWindow window(VideoMode(800, 600), "My project!");

	// Главный цикл приложения. Выполняется, пока открыто окно
	while (window.isOpen())
	{
		// Обрабатываем очередь событий в цикле
		Event event;
		while (window.pollEvent(event))
		{
			// Пользователь нажал на «крестик» и хочет закрыть окно?
			if (event.type == Event::Closed)
				// Тогда закрываем его
				window.close();
		}
		// Отрисовка окна	
		window.clear(Color(250, 220, 100, 0));
		for (int i = 0; i < 10; i++) { all_balls[i].show(&window); }
		
		window.display();
	}

	return 0;
}