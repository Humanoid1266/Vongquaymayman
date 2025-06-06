#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

using namespace std;

int interface() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Vong Quay May Man");

    sf::CircleShape wheel(200);
    wheel.setFillColor(sf::Color::Green);
    wheel.setPosition(300, 100);

    sf::RectangleShape spinButton(sf::Vector2f(100, 50));
    spinButton.setFillColor(sf::Color::Red);
    spinButton.setPosition(350, 500);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear(sf::Color::White);
        window.draw(wheel);
        window.draw(spinButton);
        window.display();
    }

    return 0;
}