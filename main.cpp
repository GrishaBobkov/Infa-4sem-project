#include <iostream>
#include <SFML/Graphics.hpp>

using namespace sf;
using namespace std;

int main()
{
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
		CircleShape ob1(40);
		ob1.setPosition({ 20, 20 });
		ob1.setFillColor(Color(0xFF, 0x0, 0x0));
		window.draw(ob1);
		window.display();
	}

	return 0;
}