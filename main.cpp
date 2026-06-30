#include "bin/object/Tetramino.hpp"
#include "bin/object/Inventory.hpp"
#include "bin/view/AppStatus.hpp"
#include "bin/view/MainMenu.hpp"
#include "bin/view/GameGUI.hpp"
#include "bin/view/GameResults.hpp"
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

int currentAppStatus = AppStatus::MAINMENU;

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Tetromino Test");
    
    // Настройки
    float cellSize = 40.0f;
    sf::Vector2f gridOffset(100, 100);
    
    TetrominoManager manager(cellSize, gridOffset);
    
    // Загрузка фигуры из JSON
    std::ifstream file("data/static/BD/item.json");
    json data;
    file >> data;
    
    // Создаем L-образную фигуру
    auto tetromino = manager.createFromJSON(data["3"]);
    
    // Сетка для коллизий
    std::vector<std::vector<bool>> grid(10, std::vector<bool>(10, false));

    Inventory inventory(8, 6);

     std::vector<std::vector<bool>> inv_grid(inventory.getGrid());
    
    while (window.isOpen()) {
        while (const std::optional event =window.pollEvent()) {
            if (event->is<sf::Event::Closed>()){
                window.close();
            }
            if (event->is<sf::Event::KeyPressed>()) {

                const auto* keyPressed = event->getIf<sf::Event::KeyPressed>();

                if (keyPressed->code == sf::Keyboard::Key::Left)
                    manager.move(tetromino, -1, 0, grid);
                if (keyPressed->code == sf::Keyboard::Key::Right)
                    manager.move(tetromino, 1, 0, grid);
                if (keyPressed->code == sf::Keyboard::Key::Down)
                    manager.move(tetromino, 0, 1, grid);
                if (keyPressed->code == sf::Keyboard::Key::Up)
                    manager.rotate(tetromino, grid);
                if (keyPressed->code == sf::Keyboard::Key::Space) {
                    // Фиксация фигуры в сетке
                    auto cells = manager.getAbsoluteCells(tetromino);
                    if (inventory.ValidInInventory(cells)){
                        for (const auto& cell : cells) {
                            if (cell.y >= 0 && cell.y < inv_grid.size() &&
                                cell.x >= 0 && cell.x < inv_grid[0].size()) {
                                grid[cell.y][cell.x] = true;
                            }
                        }
                    }
                }
            }
        }
        
        
        window.clear(sf::Color::Black);
        
        // Рисуем сетку
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
        inventory.draw(window, gridOffset, cellSize);
        
        // Рисуем активную фигуру
        manager.draw(window, tetromino);
        
        window.display();
    };

    return 0;
}