#include "../view/GameGUI.hpp"
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

const sf::Vector2f gridOffset({100.f, 200.f});
const sf::Vector2f inventoryOffset({540.f, 200.f});

// ============================================
// DRAG-AND-DROP МЕТОДЫ
// ============================================

bool GameGUI::isMouseOnInventory(sf::Vector2f mousePos) const {

    sf::FloatRect inventoryRect(
        inventoryOffset,
        sf::Vector2f(inventory.getWidth() * cellSize,
        inventory.getHeight() * cellSize)
    );
    return inventoryRect.contains(mousePos);
}

void GameGUI::startDrag(const sf::Event& event) {
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(static_cast<float>(mousePressed->position.x),
                                 static_cast<float>(mousePressed->position.y));
            
            // Проверяем, кликнули ли по фигуре
            if (manager.isPointOnTetromino(player, mousePos)) {
                // Сохраняем начальную позицию
                dragState.isDragging = true;
                dragState.draggedTetromino = &player;
                dragState.startGridPos = player.position;
                dragState.dragStartMouse = mousePos;
                
                // Находим, за какую клетку фигуры мы держим
                auto pixelPositions = player.getPixelPositions(cellSize, gridOffset);
                for (size_t i = 0; i < pixelPositions.size(); ++i) {
                    sf::Vector2f o(pixelPositions[i].x,
                        pixelPositions[i].y);

                    sf::FloatRect cellRect(o,
                        sf::Vector2f(cellSize,cellSize)
                    );
                    
                    if (cellRect.contains(mousePos)) {
                        // Сохраняем смещение от центра клетки
                        sf::Vector2f cellCenter = {
                            pixelPositions[i].x + cellSize / 2.0f,
                            pixelPositions[i].y + cellSize / 2.0f
                        };
                        dragState.mouseOffset = {
                            mousePos.x - cellCenter.x,
                            mousePos.y - cellCenter.y
                        };
                        
                        // Сохраняем, за какую клетку держим
                        dragState.grabbedCell = player.shape[i];
                        break;
                    }
                }
                
                std::cout << "Started dragging! Grabbed cell: " 
                          << dragState.grabbedCell.x << ", " 
                          << dragState.grabbedCell.y << std::endl;
            }
        }
    }
}

void GameGUI::updateDrag(const sf::Event& event) {
    if (!dragState.isDragging) return;
    
    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        sf::Vector2f mousePos(static_cast<float>(mouseMoved->position.x),
                             static_cast<float>(mouseMoved->position.y));
        
        // Находим, за какую клетку фигуры мы держим
        sf::Vector2i grabbedCell = {0, 0};
        float minDistance = std::numeric_limits<float>::max();
        
        auto pixelPositions = player.getPixelPositions(cellSize, gridOffset);
        for (size_t i = 0; i < pixelPositions.size(); ++i) {
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
                grabbedCell = player.shape[i];
            }
        }
        
        // Конвертируем позицию мыши в координаты сетки
        sf::Vector2i gridPos = manager.pixelToGrid(mousePos);
        
        // Вычисляем новую позицию фигуры
        sf::Vector2i newPosition = {
            gridPos.x - grabbedCell.x,
            gridPos.y - grabbedCell.y
        };
        
        // Обновляем позицию фигуры
        player.position = newPosition;
        
        lastMousePos = mousePos;
    }
}

void GameGUI::endDrag(const sf::Event& event) {
    if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseReleased->button == sf::Mouse::Button::Left && dragState.isDragging) {
            sf::Vector2f mousePos(static_cast<float>(mouseReleased->position.x),
                                 static_cast<float>(mouseReleased->position.y));

            std::cout << "End" << std::endl;
            
            // Проверяем, находится ли мышь над инвентарем
            if (isMouseOnInventory(mousePos)) {
                // Пытаемся разместить фигуру в инвентаре
                sf::Vector2i inventoryGridPos = manager.InvPixelToGrid(
                    sf::Vector2f(mousePos.x - inventoryOffset.x, 
                                mousePos.y - inventoryOffset.y)
                );

                
                // Корректируем позицию для инвентаря
                Tetromino temp = player;
                temp.position = inventoryGridPos;

                // Находим, за какую клетку фигуры мы держим
                sf::Vector2i grabbedCell = {0, 0};
                float minDistance = std::numeric_limits<float>::max();
        
                auto pixelPositions = player.getPixelPositions(cellSize, gridOffset);
                for (size_t i = 0; i < pixelPositions.size(); ++i) {
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
                        grabbedCell = player.shape[i];
                    }
                }
        
        
                // Вычисляем новую позицию фигуры
                sf::Vector2i newPosition = {
                    inventoryGridPos.x - grabbedCell.x,
                    inventoryGridPos.y - grabbedCell.y
                };

                temp.position = newPosition;
                
                // Получаем абсолютные клетки фигуры
                auto cells = manager.getAbsoluteCells(temp);
                
                // Проверяем, можно ли разместить в инвентаре
                if (inventory.ValidInInventory(cells)) {
                    // Размещаем в инвентаре через placeItem
                    if (inventory.placeItem(cells)) {
                        std::cout << "Item placed in inventory!" << cells[0].x << cells[0].y << std::endl;
                        
                        // Создаем новую фигуру
                        std::ifstream file("../data/static/BD/item.json");
                        if (file.is_open()) {
                            json newData;
                            file >> newData;
                            player = manager.createFromJSON(newData["2"]);
                        }
                    }
                } else {
                    // Нельзя разместить, возвращаем на начальную позицию
                    player.position = dragState.startGridPos;
                    std::cout << "Cannot place in inventory!" << std::endl;
                }
            } else {
                    // Нельзя разместить, возвращаем на начальную позицию
                    player.position = dragState.startGridPos;
                    std::cout << "Cannot place in inventory!" << std::endl;
                }
            
            // Сбрасываем состояние перетаскивания
            dragState.reset();
        }
    }
}

void GameGUI::cancelDrag() {
    if (dragState.isDragging) {
        // Возвращаем фигуру на начальную позицию
        player.position = dragState.startGridPos;
        dragState.reset();
        std::cout << "Drag cancelled" << std::endl;
    }
}