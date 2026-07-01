#include <SFML/Graphics.hpp>
#include "bin/view/AppStatus.hpp"
#include "bin/view/MainMenu.hpp"
#include "bin/view/GameGUI.hpp"
#include "bin/view/GameResults.hpp"
#include <iostream>

int currentAppStatus = AppStatus::MAINMENU;
int lastCompletedTasks = 0;

int main() {
    sf::RenderWindow window(sf::VideoMode({1600, 900}), "Inventory Folding");
    window.setFramerateLimit(60);
    
    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile("Background.jpg"))
        std::cout << "Не удалось загрузить фоновое изображение!" << std::endl;
    sf::Sprite bgSprite(bgTexture);

    sf::Vector2u windowSize = window.getSize();
    bgSprite.setScale({
        static_cast<float>(windowSize.x) / bgTexture.getSize().x,
        static_cast<float>(windowSize.y) / bgTexture.getSize().y
    });

    MainMenu mainMenu;
    GameGUI gameGUI;
    GameResults gameResults(lastCompletedTasks);

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

        window.draw(bgSprite);

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