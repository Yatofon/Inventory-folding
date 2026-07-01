#pragma once

#include "Tetramino.hpp"
#include <vector>
#include <SFML/Graphics.hpp>

struct StoredItem {
    Tetromino tetromino;
    sf::Vector2i position;
    sf::Color color;
    std::vector<sf::Vector2i> cells;
};

class Inventory {
private:
    std::vector<std::vector<bool>> grid;
    std::vector<StoredItem> storedItems; // Храним все предметы с их цветами
    int width;
    int height;
    sf::Color emptyColor;
    sf::Color occupiedColor;
    
public:
    Inventory(int w, int h, sf::Color empty = sf::Color::Transparent, 
              sf::Color occupied = sf::Color(100, 100, 100))
        : width(w), height(h), emptyColor(empty), occupiedColor(occupied) {
        grid.resize(height, std::vector<bool>(width, false));
    }
    

    bool placeItem(int x, int y);
    bool placeItem(const std::vector<sf::Vector2i>& cells);

    // Попытка добавить предмет
    bool addItem(const Tetromino& item, sf::Vector2i position);

    // Удаление предмета (очистка клеток)
    bool removeItem(const Tetromino& item, sf::Vector2i position);

    // Очистка инвентаря
    void clear();

    // Отрисовка инвентаря
    void draw(sf::RenderWindow& window, sf::Vector2f position, float cellSize) const;

    bool ValidInInventory(std::vector<sf::Vector2i>& cells) const;

    // Геттеры
    int getWidth() const;
    int getHeight() const;
    std::vector<std::vector<bool>> getGrid() const;
    bool isOccupied(int x, int y) const;

    // Новый метод для размещения с сохранением цвета
    bool placeItemWithColor(const Tetromino& tetromino, 
                           const std::vector<sf::Vector2i>& cells,
                           sf::Color color);
    
    // Получить все сохраненные предметы
    const std::vector<StoredItem>& getStoredItems() const { return storedItems; }

    // Сохранение в JSON
    json toJSON() const;

    // Загрузка из JSON
    static Inventory fromJSON(const json& data) {
        int width = data["width"].get<int>();
        int height = data["height"].get<int>();
        Inventory inv(width, height);
        
        auto gridData = data["grid"];
        for (int y = 0; y < height && y < gridData.size(); ++y) {
            for (int x = 0; x < width && x < gridData[y].size(); ++x) {
                inv.grid[y][x] = gridData[y][x].get<int>() == 1;
            }
        }
        
        return inv;
    };

};