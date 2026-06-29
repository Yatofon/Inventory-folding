#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Структура Tetromino

struct Tetromino {
    std::vector<sf::Vector2i> shape;  // относительные координаты клеток
    sf::Vector2i position;             // позиция в сетке (в клетках)
    sf::Color color;                   // цвет фигуры
    
    // Получение реальных пиксельных координат для отрисовки
    std::vector<sf::Vector2f> getPixelPositions(float cellSize, sf::Vector2f offset) const;
};


// Класс TetrominoManager
class TetrominoManager {
private:
    float cellSize;           // размер одной клетки в пикселях
    sf::Vector2f gridOffset;  // смещение сетки на экране
    
    // Вспомогательные методы
    std::vector<sf::Vector2i> rotateShape(const std::vector<sf::Vector2i>& shape);
    std::vector<sf::Vector2i> normalizeShape(const std::vector<sf::Vector2i>& shape);

public:
    TetrominoManager(float cellSize, sf::Vector2f gridOffset) 
        : cellSize(cellSize), gridOffset(gridOffset) {}
    
    // Создание тетрамино из JSON
    Tetromino createFromJSON(const json& data);
    
    // Отрисовка тетрамино
    void draw(sf::RenderWindow& window, const Tetromino& tetromino, 
              const sf::Texture* texture = nullptr);
    
    // Проверка коллизий с сеткой
    bool isValidPosition(const Tetromino& tetromino, 
                        const std::vector<std::vector<bool>>& grid);
    
    // Перемещение тетрамино
    bool move(Tetromino& tetromino, int dx, int dy, 
              const std::vector<std::vector<bool>>& grid);
    
    // Поворот тетрамино с wall kick
    bool rotate(Tetromino& tetromino, 
                const std::vector<std::vector<bool>>& grid);
    
    // Получение всех клеток фигуры в абсолютных координатах сетки
    std::vector<sf::Vector2i> getAbsoluteCells(const Tetromino& tetromino);
};