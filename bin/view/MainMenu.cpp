#include "MainMenu.hpp"
#include "AppStatus.hpp"
#include <SFML/Graphics.hpp>

void MainMenu(sf::RenderWindow& window) {
    sf::Font font;
    bool fontLoaded = font.openFromFile("Tokushupikuseru-Regular.otf");

    if (fontLoaded) {
        sf::Text text(font, "MAIN MENU", 60);
        text.setPosition({100, 100});
        text.setFillColor(sf::Color::White);
        window.draw(text);

        sf::Text btnText(font, "Start", 30);
        btnText.setPosition({120, 205});
        btnText.setFillColor(sf::Color::Black);
        window.draw(btnText);
    }

    sf::RectangleShape btn({200, 50});
    btn.setPosition({100, 200});
    btn.setFillColor(sf::Color::Green);
    window.draw(btn);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (btn.getGlobalBounds().contains({(float)mousePos.x, (float)mousePos.y})) {
            currentAppStatus = AppStatus::GAMEGUI;
        }
    }
}