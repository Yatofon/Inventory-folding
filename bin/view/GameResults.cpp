    #include "GameResults.hpp"
    #include "AppStatus.hpp"
    #include <SFML/Graphics.hpp>
    #include "GameStats.hpp"

    namespace GameStats {
        int totalCost = 0;
        int itemCount = 0;
        int usedCells = 0;
        int totalCells = 48;
    }

    GameResults::GameResults():
        fontLoaded(false)
    {
        fontLoaded = font.openFromFile("Tokushupikuseru-Regular.otf");
        if (fontLoaded) 
        {
            title = std::make_unique<sf::Text>(font, "Results", 120);
            title->setPosition({600.f, -50.f});
            title->setFillColor(sf::Color::Black);

            totalCostText = std::make_unique<sf::Text>(font, "Total coas is ", 60);
            totalCostText->setPosition({50.f, 100.f});
            totalCostText->setFillColor(sf::Color::Black);

            itemCountText = std::make_unique<sf::Text>(font, "Total cells is ", 60);
            itemCountText->setPosition({50.f, 180.f});
            itemCountText->setFillColor(sf::Color::Black);

            usedCellsText = std::make_unique<sf::Text>(font, "Used cells ", 60);
            usedCellsText->setPosition({50.f, 260.f});
            usedCellsText->setFillColor(sf::Color::Black);

            tryAgainText = std::make_unique<sf::Text>(font, "Try again", 60);
            tryAgainText->setPosition({270.f, 790.f});
            tryAgainText->setFillColor(sf::Color::Black);

            mainMenuText = std::make_unique<sf::Text>(font, "Main menu", 60);
            mainMenuText->setPosition({1030.f, 790.f});
            mainMenuText->setFillColor(sf::Color::Black);
        }

        tryAgainBtn.setSize({300, 80});
        tryAgainBtn.setPosition({270.f, 800.f});
        tryAgainBtn.setFillColor(sf::Color::Magenta);

        mainMenuBtn.setSize({300, 80});
        mainMenuBtn.setPosition({1030.f, 800.f});
        mainMenuBtn.setFillColor(sf::Color(255, 150, 150));
    }

    void GameResults::handleEvent(const sf::Event& event, sf::RenderWindow& window)
    {
        if (event.is<sf::Event::MouseButtonPressed>())
        {
            auto mouse = event.getIf<sf::Event::MouseButtonPressed>();
            if (mouse->button == sf::Mouse::Button::Left)
            {
                sf::Vector2f mousePos(static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y));
                if (tryAgainBtn.getGlobalBounds().contains(mousePos))
                    currentAppStatus = AppStatus::GAMEGUI;
                if (mainMenuBtn.getGlobalBounds().contains(mousePos)) {
                    currentAppStatus = AppStatus::MAINMENU;
                }
            }
        }
    }

    void GameResults::render(sf::RenderWindow& window)
    {
        if (fontLoaded && totalCostText && itemCountText && usedCellsText) 
        {
            totalCostText->setString("Total cost: " + std::to_string(GameStats::totalCost));
            itemCountText->setString("Items: " + std::to_string(GameStats::itemCount));
            usedCellsText->setString("Used cells: " + std::to_string(GameStats::usedCells) + "/" + std::to_string(GameStats::totalCells));
        }
        window.draw(tryAgainBtn);
        window.draw(mainMenuBtn);

        if (fontLoaded)
        {
            if (title) window.draw(*title);
            if (totalCostText) window.draw(*totalCostText);
            if (itemCountText) window.draw(*itemCountText);
            if (usedCellsText) window.draw(*usedCellsText);
            if (tryAgainText) window.draw(*tryAgainText);
            if (mainMenuText) window.draw(*mainMenuText);
        }
    }