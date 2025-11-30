
#include <SFML/Graphics.hpp>
#include “ComplexPlane.h”
#include <iostream>




using namespace sf;
using namespace std;


int main()
{

	int pixelWidth = VideoMode::getDesktopMode().width;
	int pixelHeight = VideoMode::getDesktopMode().height;

	RenderWindow window(VideoMode(pixelWidth, pixelHeight), "Mandelbrot");
	ComplexPlane plane(pixelWidth, pixelHeight);



	//font appear NOW
	sf::Font font;
	if (!font.loadFromFile("arial.ttf"))
	{
		cout << "font isn't real sorry.";
	}

	sf::Text text;
	text.setFont(font);
	text.setString("filler");
	text.setCharacterSize(30);
	text.setFillColor(sf::Color::White);
	text.setStyle(sf::Text::Bold);
	text.setPosition(20, 20);

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				// Quit the game when the window is closed
				window.close();
			}
			if (event.type == sf::Event::MouseButtonPressed)
			{
				Vector2i mouse(event.mouseMove.x, event.mouseMove.y);

				if (event.mouseButton.button == sf::Mouse::Right)
				{
					//Right click will zoomOut and call setCenter on the 
		//ComplexPlane object with the (x,y) pixel location of the mouse click
					plane.zoomOut();
					plane.setCenter(mouse);


				}
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					//Left click will zoomIn and call setCenter on the 
//ComplexPlane object with the (x,y) pixel location of the //mouse click
					plane.zoomIn();
					plane.setCenter(mouse);

				}

			}

			if (event.type == sf::Event::MouseMoved)
			{
				//Call setMouseLocation on the ComplexPlane object to store the 
		//(x,y) pixel location of the mouse click
				Vector2i mouse(event.mouseButton.x, event.mouseButton.y);
				plane.setMouseLocation(mouse);
				//This will be used later to display the mouse coordinates as it moves
			}
		}


		if (Keyboard::isKeyPressed(Keyboard::Escape))

		{

			window.close();

		}




		///Update
		plane.updateRender();
		plane.loadText(text);


		///Draw

		window.clear();
		window.draw(plane);
		window.draw(text);
		window.display();

	}
}

