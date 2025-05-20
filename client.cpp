#include <SFML/Graphics.hpp>
#include <iostream>

int main() {
    // SFML window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Vong Quay May Man");
    // wheel static
    sf::circleShape wheel(200);
    wheel.setFillColor(sf::yellow::green);
    wheel.setPosition(300, 200);
    //spin button
    sf::Reactangleshape spinbutton(sf::vector2f(100, 50));
    spinbutton.setFillColor(sf::Color::Red);
    spinbutton.setPosition(350, 500);
}