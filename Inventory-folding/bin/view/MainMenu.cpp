#include "MainMenu.hpp"
#include "AppStatus.hpp"
#include <iostream>
MainMenu::MainMenu()
{
    fontLoaded = font.openFromFile("Tokushupikuseru-Regular.otf");
    
    if (!startTexture.loadFromFile("BtnBackground.jpg")) {
        std::cout << "Не удалось загрузить текстуру Start!" << std::endl;
    }
    if (!exitTexture.loadFromFile("BtnBackground.jpg")) {
        std::cout << "Не удалось загрузить текстуру Exit!" << std::endl;
    }

    float winWidth = 1600.0;
    float winHeight = 900.0;
    
    if (fontLoaded)
    {
        title = std::make_unique<sf::Text>(font, "Inventory Folding", 120);
        title->setPosition({520.f, 150.f});
        title->setFillColor(sf::Color::White);

        startText = std::make_unique<sf::Text>(font, "START", 60);
        startText->setPosition({755.f, 440.f});
        startText->setFillColor(sf::Color::Black);

        exitText = std::make_unique<sf::Text>(font, "EXIT", 60);
        exitText->setPosition({765.f, 590.f});
        exitText->setFillColor(sf::Color::Black);
    }
    startBtn.setSize({300, 80});
    startBtn.setPosition({650.f, 450.f});
    startBtn.setTexture(&startTexture);

    exitBtn.setSize({300, 80});
    exitBtn.setPosition({650.f, 600.f});
    exitBtn.setTexture(&exitTexture);
}

void MainMenu::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if(event.is<sf::Event::MouseButtonPressed>())
    {
        auto mouse = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouse->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mousePos(static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y));
            if (startBtn.getGlobalBounds().contains(mousePos))
                currentAppStatus = AppStatus::GAMEGUI;
            if (exitBtn.getGlobalBounds().contains(mousePos))
                window.close();
        }  
    }
}

void MainMenu::render(sf::RenderWindow& window)
{
    window.draw(startBtn);
    window.draw(exitBtn);
    if (fontLoaded)
    {
        if (title) window.draw(*title);
        if (startText) window.draw(*startText);
        if (exitText) window.draw(*exitText);
    }
}