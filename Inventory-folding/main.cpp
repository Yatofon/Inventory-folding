#include <SFML/Graphics.hpp>
#include "view/AppStatus.hpp"
#include "view/MainMenu.hpp"
#include "view/GameGUI.hpp"
#include "view/GameResults.hpp"

int currentAppStatus = AppStatus::MAINMENU;

int main() {
    sf::RenderWindow window(sf::VideoMode({1800, 900}), "Inventory Folding");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::Black);

        switch (currentAppStatus) {
            case AppStatus::MAINMENU:
                MainMenu(window);
                break;
            case AppStatus::GAMEGUI:
                GameGUI(window);
                break;
            case AppStatus::GAMERESULTS:
                GameResults(window);
                break;
        }

        window.display();
    }

    return 0;
}