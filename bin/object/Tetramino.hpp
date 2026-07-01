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

// Структура для состояния drag-and-drop
struct DragState {
    bool isDragging = false;
    sf::Vector2i startGridPos;     // Позиция в сетке до начала перетаскивания
    sf::Vector2f mouseOffset;       // Смещение мыши относительно центра клетки
    sf::Vector2i grabbedCell;       // Какая клетка фигуры захвачена
    Tetromino* draggedTetromino = nullptr;
    sf::Vector2f dragStartMouse;    // Позиция мыши в момент начала drag
    
    void reset() {
        isDragging = false;
        draggedTetromino = nullptr;
        grabbedCell = {0, 0};
    }
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

    void drawWithAlpha(sf::RenderWindow& window, const Tetromino& tetromino, 
                                     float alpha);

     // Метод для отрисовки с эффектом перетаскивания
    void drawDragging(sf::RenderWindow& window, const Tetromino& tetromino,
                      sf::Color highlightColor = sf::Color::Yellow,
                      float outlineThickness = 3.0f);
    
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

    // Методы для работы с drag-and-drop
    bool isPointOnTetromino(const Tetromino& tetromino, sf::Vector2f mousePos) const;
    sf::Vector2i pixelToGrid(sf::Vector2f pixelPos) const;
    sf::Vector2f gridToPixel(sf::Vector2i gridPos) const;
    bool snapToGrid(Tetromino& tetromino, sf::Vector2f mousePos, 
                    const std::vector<std::vector<bool>>& grid) ;
};