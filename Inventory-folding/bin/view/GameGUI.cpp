#include "GameGUI.hpp"
#include "AppStatus.hpp"
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "GameStats.hpp"

const sf::Vector2f gridOffset({100.f, 200.f});
const sf::Vector2f inventoryOffset({600.f, 200.f});

std::ifstream file("../data/static/BD/item.json");

GameGUI::GameGUI()
    :
    inventory(8, 6, sf::Color::Transparent, sf::Color::White),
    manager(40.f, {100.f, 200.f}),
    grid(10, std::vector<bool>(10, false)),
    cellSize(40.0f),
    title(nullptr),
    backText(nullptr),
    endText(nullptr)
{
    fontLoaded = font.openFromFile("Tokushupikuseru-Regular.otf");

    std::ifstream file("../data/static/BD/item.json");
    file >> data;
    player = manager.createFromJSON(data["2"]);
    
    float winWidth = 1600.f;
    float winHeight = 900.f;

    if (fontLoaded)
    {
        title = std::make_unique<sf::Text>(font, "Game Screen", 120);
        title->setPosition({600.f, -50.f});
        title->setFillColor(sf::Color::Black);

        backText = std::make_unique<sf::Text>(font, "Back", 60);
        backText->setPosition({130.f, 790.f});
        backText->setFillColor(sf::Color::Black);

        endText = std::make_unique<sf::Text>(font, "End", 60);
        endText->setPosition({1400.f, 790.f});
        endText->setFillColor(sf::Color::Black);
    }

    backBtn.setSize({300, 80});
    backBtn.setPosition({20.f, 800.f});
    backBtn.setFillColor(sf::Color::Magenta);

    endBtn.setSize({300, 80});
    endBtn.setPosition({1280.f, 800.f});
    endBtn.setFillColor(sf::Color(255, 150, 150));
}

GameGUI::~GameGUI() = default;

void GameGUI::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
     if (event.is<sf::Event::KeyPressed>()) {
        auto key = event.getIf<sf::Event::KeyPressed>();
        if (key) {
            switch (key->code) {
                case sf::Keyboard::Key::Left:
                    manager.move(player, -1, 0, grid);
                    break;
                case sf::Keyboard::Key::Right:
                    manager.move(player, 1, 0, grid);
                    break;
                case sf::Keyboard::Key::Down:
                    manager.move(player, 0, 1, grid);
                    break;
                case sf::Keyboard::Key::Up:
                    manager.move(player, 0, -1, grid);
                    break;
                case sf::Keyboard::Key::LShift:
                    manager.rotate(player, grid);
                    break;
                case sf::Keyboard::Key::Space: {
                    auto cells = manager.getAbsoluteCells(player);
                    if (inventory.ValidInInventory(cells)) {
                        for (const auto& cell : cells) {
                            if (cell.y >= 0 && cell.y < (int)grid.size() &&
                                cell.x >= 0 && cell.x < (int)grid[0].size()) {
                                grid[cell.y][cell.x] = true;
                            }
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }


    if (event.is<sf::Event::MouseButtonPressed>())
    {
        auto mouse = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouse->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mousePos(static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y));
            if (backBtn.getGlobalBounds().contains(mousePos))
            {
                GameStats::totalCost = 100;
                GameStats::itemCount = 5;
                GameStats::usedCells = 12;
                GameStats::totalCells = 48;
                currentAppStatus = AppStatus::MAINMENU;
            }
                if (endBtn.getGlobalBounds().contains(mousePos))
                currentAppStatus = AppStatus::GAMERESULTS;
        }
    } 
}

void GameGUI::render(sf::RenderWindow& window)
{
    for (int y = 0; y < grid.size(); ++y) {
        for (int x = 0; x < grid[0].size(); ++x) {
            sf::RectangleShape cell(sf::Vector2f(cellSize - 1, cellSize - 1));
            sf::Vector2f pos(gridOffset.x + x * cellSize, 
                            gridOffset.y + y * cellSize);
            cell.setPosition(pos);
            
            if (grid[y][x]) {
                cell.setFillColor(sf::Color::Blue);
            } else {
                cell.setFillColor(sf::Color::Transparent);
            }
            cell.setOutlineColor(sf::Color::White);
            cell.setOutlineThickness(1);
            window.draw(cell);
        }
    }

    // Рисуем сетку
    inventory.draw(window, inventoryOffset, cellSize);
    
    // Рисуем активную фигуру
    manager.draw(window, player);
    
    window.draw(backBtn);
    window.draw(endBtn);
    if (fontLoaded) {
        if (title) window.draw(*title);
        if (backText) window.draw(*backText);
        if (endText) window.draw(*endText);
    }
}