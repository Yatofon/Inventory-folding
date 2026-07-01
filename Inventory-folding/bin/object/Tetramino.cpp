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

    void TetrominoManager::drawWithAlpha(sf::RenderWindow& window, const Tetromino& tetromino, 
                                     float alpha) {
    if (tetromino.shape.empty() || cellSize <= 0.0f) {
        return;
    }
    
    auto pixelPositions = tetromino.getPixelPositions(cellSize, gridOffset);
    if (pixelPositions.empty()) {
        return;
    }
    
    sf::Color colorWithAlpha = tetromino.color;
    // Используем std::uint8_t вместо sf::Uint8
    colorWithAlpha.a = static_cast<std::uint8_t>(std::clamp(alpha, 0.0f, 255.0f));
    
    for (const auto& pos : pixelPositions) {
        sf::RectangleShape cell(sf::Vector2f(cellSize - 1, cellSize - 1));
        cell.setPosition(pos);
        cell.setFillColor(colorWithAlpha);
        cell.setOutlineColor(sf::Color::White);
        cell.setOutlineThickness(0.5f);
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

    // ============================================
// DRAG-AND-DROP МЕТОДЫ
// ============================================

bool TetrominoManager::isPointOnTetromino(const Tetromino& tetromino, sf::Vector2f mousePos) const {
    if (tetromino.shape.empty()) return false;
    
    auto pixelPositions = tetromino.getPixelPositions(cellSize, gridOffset);
    
    for (const auto& pos : pixelPositions) {
        sf::Vector2f position(pos.x, pos.y);
        sf::Vector2f size(cellSize, cellSize);
        sf::FloatRect cellRect(position, size);
        if (cellRect.contains(mousePos)) {
            return true;
        }
    }
    return false;
}

sf::Vector2i TetrominoManager::pixelToGrid(sf::Vector2f pixelPos) const {
    int gridX = static_cast<int>(std::floor((pixelPos.x - gridOffset.x) / cellSize));
    int gridY = static_cast<int>(std::floor((pixelPos.y - gridOffset.y) / cellSize));
    return {gridX, gridY};
}

sf::Vector2f TetrominoManager::gridToPixel(sf::Vector2i gridPos) const {
    float pixelX = gridOffset.x + gridPos.x * cellSize;
    float pixelY = gridOffset.y + gridPos.y * cellSize;
    return {pixelX, pixelY};
}

bool TetrominoManager::snapToGrid(Tetromino& tetromino, sf::Vector2f mousePos, 
                                  const std::vector<std::vector<bool>>& grid) {
    if (tetromino.shape.empty()) return false;
    
    // Находим bounding box фигуры
    int minX = tetromino.shape[0].x;
    int minY = tetromino.shape[0].y;
    int maxX = tetromino.shape[0].x;
    int maxY = tetromino.shape[0].y;
    for (const auto& cell : tetromino.shape) {
        minX = std::min(minX, cell.x);
        minY = std::min(minY, cell.y);
        maxX = std::max(maxX, cell.x);
        maxY = std::max(maxY, cell.y);
    }
    
    // Получаем позицию мыши в координатах сетки
    sf::Vector2i gridPos = pixelToGrid(mousePos);
    
    // Вычисляем, какая клетка фигуры ближе всего к позиции мыши
    // Для этого находим клетку фигуры, на которую наведена мышь
    sf::Vector2i closestCell = {0, 0};
    float minDistance = std::numeric_limits<float>::max();
    
    auto pixelPositions = tetromino.getPixelPositions(cellSize, gridOffset);
    for (size_t i = 0; i < pixelPositions.size(); ++i) {
        // Центр клетки в пикселях
        sf::Vector2f cellCenter = {
            pixelPositions[i].x + cellSize / 2.0f,
            pixelPositions[i].y + cellSize / 2.0f
        };
        
        float distance = std::sqrt(
            std::pow(mousePos.x - cellCenter.x, 2) +
            std::pow(mousePos.y - cellCenter.y, 2)
        );
        
        if (distance < minDistance) {
            minDistance = distance;
            closestCell = tetromino.shape[i];
        }
    }
    
    // Теперь вычисляем новую позицию фигуры
    // Чтобы клетка, на которую наведена мышь, оказалась в позиции gridPos
    sf::Vector2i newPosition = {
        gridPos.x - closestCell.x,
        gridPos.y - closestCell.y
    };
    
    // Пробуем разные варианты привязки (для удобства)
    std::vector<sf::Vector2i> offsets = {
        {0, 0}, {1, 0}, {-1, 0}, {0, 1}, {0, -1},
        {1, 1}, {-1, 1}, {1, -1}, {-1, -1}
    };
    
    Tetromino temp = tetromino;
    
    for (const auto& offset : offsets) {
        temp.position = {
            newPosition.x + offset.x,
            newPosition.y + offset.y
        };
        
        if (isValidPosition(temp, grid)) {
            tetromino.position = temp.position;
            return true;
        }
    }
    
    return false;
}

void TetrominoManager::drawDragging(sf::RenderWindow& window, const Tetromino& tetromino,
                                    sf::Color highlightColor, float outlineThickness) {
    if (tetromino.shape.empty() || cellSize <= 0.0f) {
        return;
    }
    
    auto pixelPositions = tetromino.getPixelPositions(cellSize, gridOffset);
    if (pixelPositions.empty()) {
        return;
    }
    
    // Рисуем тень (полупрозрачную копию)
    sf::Color shadowColor = tetromino.color;
    shadowColor.a = 100;
    
    for (const auto& pos : pixelPositions) {
        // Основная клетка
        sf::RectangleShape cell(sf::Vector2f(cellSize - 2, cellSize - 2));
        cell.setPosition(sf::Vector2f(pos.x + 1, pos.y + 1));
        cell.setFillColor(tetromino.color);
        cell.setOutlineColor(highlightColor);
        cell.setOutlineThickness(outlineThickness);
        window.draw(cell);
        
        // Эффект свечения (дополнительная обводка)
        sf::RectangleShape glow(sf::Vector2f(cellSize + 4, cellSize + 4));
        glow.setPosition(sf::Vector2f(pos.x - 2, pos.y - 2));
        glow.setFillColor(sf::Color::Transparent);
        glow.setOutlineColor(sf::Color(highlightColor.r, highlightColor.g, highlightColor.b, 50));
        glow.setOutlineThickness(2);
        window.draw(glow);
    }
}