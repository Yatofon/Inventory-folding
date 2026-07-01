#include <SFML/Graphics.hpp>
#include "bin/view/AppStatus.hpp"
#include "bin/view/MainMenu.hpp"
#include "bin/view/GameGUI.hpp"
#include "bin/view/GameResults.hpp"

int currentAppStatus = AppStatus::MAINMENU;

int main() {
    sf::RenderWindow window(sf::VideoMode({1600, 900}), "Inventory Folding");
    window.setFramerateLimit(60);
    
    MainMenu mainMenu;
    GameGUI gameGUI;
    GameResults gameResults;

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (currentAppStatus == AppStatus::MAINMENU)
                mainMenu.handleEvent(*event, window);
            if (currentAppStatus == AppStatus::GAMEGUI)
                gameGUI.handleEvent(*event, window);
            if (currentAppStatus == AppStatus::GAMERESULTS)
                gameResults.handleEvent(*event, window);
        }

        window.clear(sf::Color(255, 150, 250));

        if (currentAppStatus == AppStatus::MAINMENU)
            mainMenu.render(window);
        else if (currentAppStatus == AppStatus::GAMEGUI)
            gameGUI.render(window);
        else if (currentAppStatus == AppStatus::GAMERESULTS)
            gameResults.render(window);

        window.display();
    }

    return 0;
}