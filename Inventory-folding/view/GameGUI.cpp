#include "GameGUI.hpp"
#include "AppStatus.hpp"
#include <SFML/Graphics.hpp>

void GameGUI(sf::RenderWindow& window) {
    sf::Font font;
    bool fontLoaded = font.openFromFile("Tokushupikuseru-Regular.otf");

    if (fontLoaded) {
        sf::Text text(font, "GAME SCREEN", 60);
        text.setPosition({100, 100});
        text.setFillColor(sf::Color::White);
        window.draw(text);

        sf::Text backText(font, "Back", 30);
        backText.setPosition({120, 205});
        backText.setFillColor(sf::Color::White);
        window.draw(backText);
    }

    sf::RectangleShape backBtn({200, 50});
    backBtn.setPosition({100, 200});
    backBtn.setFillColor(sf::Color::Red);
    window.draw(backBtn);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (backBtn.getGlobalBounds().contains({(float)mousePos.x, (float)mousePos.y})) {
            currentAppStatus = AppStatus::MAINMENU;
        }
    }
}