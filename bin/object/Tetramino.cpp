#include "Tetramino.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;


    
    // Реальные пиксельные координаты для отрисовки
    std::vector<sf::Vector2f> Tetromino::getPixelPositions(float cellSize, sf::Vector2f offset) const {
        std::vector<sf::Vector2f> pixelPos;
        for (const auto& cell : shape) {
            float x = offset.x + (position.x + cell.x) * cellSize;
            float y = offset.y + (position.y + cell.y) * cellSize;
            pixelPos.push_back({x, y});
        }
        return pixelPos;
    };


    
    // Поворот фигуры (поворот на 90 градусов по часовой стрелке)
    std::vector<sf::Vector2i> TetrominoManager::rotateShape(const std::vector<sf::Vector2i>& shape) {
        std::vector<sf::Vector2i> rotated;
        
        // Находим размеры фигуры
        int maxX = 0, maxY = 0;
        for (const auto& cell : shape) {
            maxX = std::max(maxX, cell.x);
            maxY = std::max(maxY, cell.y);
        }
        
        // Поворот: (x,y) -> (maxY - y, x)
        for (const auto& cell : shape) {
            rotated.push_back({maxY - cell.y, cell.x});
        }
        
        // Нормализуем координаты (сдвигаем к (0,0))
        return normalizeShape(rotated);
    }
    
    // Нормализация фигуры (сдвиг к минимальным координатам)
    std::vector<sf::Vector2i> TetrominoManager::normalizeShape(const std::vector<sf::Vector2i>& shape) {
        if (shape.empty()) return {};
        
        int minX = shape[0].x, minY = shape[0].y;
        for (const auto& cell : shape) {
            minX = std::min(minX, cell.x);
            minY = std::min(minY, cell.y);
        }
        
        std::vector<sf::Vector2i> normalized;
        for (const auto& cell : shape) {
            normalized.push_back({cell.x - minX, cell.y - minY});
        }
        return normalized;
    }

    
    // Создание тетрамино из JSON
    Tetromino TetrominoManager::createFromJSON(const json& data) {
        Tetromino tetromino;
        tetromino.color = sf::Color( 208, 7, 5);
        
        // Парсим форму из [[0,0],[1,0],...]
        for (const auto& coord : data["shape"]) {
            tetromino.shape.push_back({
                coord[0].get<int>(),
                coord[1].get<int>()
            });
        }
        
        tetromino.position = {0, 0};
        return tetromino;
    }
    
    // Отрисовка тетрамино
    void TetrominoManager::draw(sf::RenderWindow& window, const Tetromino& tetromino, 
              const sf::Texture* texture) {
        auto pixelPositions = tetromino.getPixelPositions(cellSize, gridOffset);
        
        for (const auto& pos : pixelPositions) {
            sf::RectangleShape cell(sf::Vector2f(cellSize - 1, cellSize - 1));
            cell.setPosition(pos);
            cell.setFillColor(tetromino.color);
            
            if (texture) {
                cell.setTexture(texture);
            }
            
            cell.setOutlineColor(sf::Color::White);
            cell.setOutlineThickness(1);
            
            window.draw(cell);
        }
    }
    
    // Проверка коллизий с сеткой
    bool TetrominoManager::isValidPosition(const Tetromino& tetromino, 
                        const std::vector<std::vector<bool>>& grid) {
        for (const auto& cell : tetromino.shape) {
            int gridX = tetromino.position.x + cell.x;
            int gridY = tetromino.position.y + cell.y;
            
            // Проверяем выход за границы
            if (gridX < 0 || gridX >= grid[0].size() || 
                gridY < 0 || gridY >= grid.size()) {
                return false;
            }
            
            // Проверяем занятость клетки
            if (grid[gridY][gridX]) {
                return false;
            }
        }
        return true;
    }
    
    // Перемещение тетрамино
    bool TetrominoManager::move(Tetromino& tetromino, int dx, int dy, 
              const std::vector<std::vector<bool>>& grid) {
        Tetromino temp = tetromino;
        temp.position.x += dx;
        temp.position.y += dy;
        
        if (isValidPosition(temp, grid)) {
            tetromino.position = temp.position;
            return true;
        }
        return false;
    }
    
    // Поворот тетрамино
    bool TetrominoManager::rotate(Tetromino& tetromino, 
                const std::vector<std::vector<bool>>& grid) {
        Tetromino temp = tetromino;
        temp.shape = rotateShape(tetromino.shape);
        
        // Проверяем 5 позиций для "wall kick" (отскок от стен)
        std::vector<sf::Vector2i> kickOffsets = {
            {0, 0}, {1, 0}, {-1, 0}, {0, 1}, {-1, 1}
        };
        
        for (const auto& offset : kickOffsets) {
            temp.position = tetromino.position;
            temp.position.x += offset.x;
            temp.position.y += offset.y;
            
            if (isValidPosition(temp, grid)) {
                tetromino.shape = temp.shape;
                tetromino.position = temp.position;
                return true;
            }
        }
        return false;
    }
    
    // Получение всех клеток фигуры в абсолютных координатах сетки
    std::vector<sf::Vector2i> TetrominoManager::getAbsoluteCells(const Tetromino& tetromino) {
        std::vector<sf::Vector2i> cells;
        for (const auto& cell : tetromino.shape) {
            cells.push_back({
                tetromino.position.x + cell.x,
                tetromino.position.y + cell.y
            });
        }
        return cells;
    };