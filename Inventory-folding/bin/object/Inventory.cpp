#include "Tetramino.hpp"
#include "Inventory.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
    
    
    // Попытка добавить предмет
    bool Inventory::addItem(const Tetromino& item, sf::Vector2i position) {
        Tetromino temp = item;
        temp.position = position;
        
        TetrominoManager manager(40, {0, 0});
        if (manager.isValidPosition(temp, grid)) {
            auto cells = manager.getAbsoluteCells(temp);
            for (const auto& cell : cells) {
                if (cell.y >= 0 && cell.y < height && 
                    cell.x >= 0 && cell.x < width) {
                    grid[cell.y][cell.x] = true;
                }
            }
            return true;
        }
        return false;
    }

    bool Inventory::placeItem(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return false;
    }
    if (grid[y][x]) {
        return false; // Клетка уже занята
    }
    grid[y][x] = true;
    return true;
}

// Размещение предмета по списку клеток
bool Inventory::placeItem(const std::vector<sf::Vector2i>& cells) {
    // Сначала проверяем, можно ли разместить
    for (const auto& cell : cells) {
        if (cell.x < 0 || cell.x >= width || 
            cell.y < 0 || cell.y >= height) {
            return false;
        }
        if (grid[cell.y][cell.x]) {
            return false; // Клетка уже занята
        }
    }
    
    // Размещаем
    for (const auto& cell : cells) {
        grid[cell.y][cell.x] = true;
    }
    return true;
}
    
    // Удаление предмета (очистка клеток)
    bool Inventory::removeItem(const Tetromino& item, sf::Vector2i position) {
        Tetromino temp = item;
        temp.position = position;
        
        TetrominoManager manager(40, {0, 0});
        auto cells = manager.getAbsoluteCells(temp);
        
        bool anyOccupied = false;
        for (const auto& cell : cells) {
            if (cell.y >= 0 && cell.y < height && 
                cell.x >= 0 && cell.x < width) {
                if (grid[cell.y][cell.x]) {
                    anyOccupied = true;
                    grid[cell.y][cell.x] = false;
                }
            }
        }
        return anyOccupied;
    }
    
    
    // Очистка инвентаря
    void Inventory::clear() {
        for (auto& row : grid) {
            std::fill(row.begin(), row.end(), false);
        }
    }
    
    // Отрисовка инвентаря
    void Inventory::draw(sf::RenderWindow& window, sf::Vector2f position, float cellSize) const {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                sf::RectangleShape cell(sf::Vector2f(cellSize - 1, cellSize - 1));
                sf::Vector2f pos(position.x + x * cellSize, 
                               position.y + y * cellSize);
                cell.setPosition(pos);
                
                if (grid[y][x]) {
                    cell.setFillColor(occupiedColor);
                } else {
                    cell.setFillColor(emptyColor);
                }
                
                cell.setOutlineColor(sf::Color::Green);
                cell.setOutlineThickness(1);
                window.draw(cell);
            }
        }
    }

    //Находится ли предмет в инвентаре
    bool Inventory::ValidInInventory(std::vector<sf::Vector2i>& cells) const{
        for (const auto& cell : cells) {
            if (cell.x < 0 || cell.x > width ||
                cell.y < 0 || cell.y > height){
                    return false;
            }

            if (grid[cell.y][cell.x]) {
                return false; // Клетка уже занята
            }
        }
        return true;
    };
    
    // Геттеры
    int Inventory::getWidth() const { return width; }
    int Inventory::getHeight() const { return height; }
    std::vector<std::vector<bool>> Inventory::getGrid() const { return grid; }
    bool Inventory::isOccupied(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) return true;
        return grid[y][x];
    }
    
    // Сохранение в JSON
    json Inventory::toJSON() const {
        json data;
        data["width"] = width;
        data["height"] = height;
        
        std::vector<std::vector<int>> gridData;
        for (const auto& row : grid) {
            std::vector<int> rowData;
            for (bool cell : row) {
                rowData.push_back(cell ? 1 : 0);
            }
            gridData.push_back(rowData);
        }
        data["grid"] = gridData;
        
        return data;
    }