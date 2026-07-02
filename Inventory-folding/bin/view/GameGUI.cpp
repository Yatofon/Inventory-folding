#include "GameGUI.hpp"
#include "AppStatus.hpp"
#include <SFML/Graphics.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "GameStats.hpp"
#include <set>
#include <sstream>
#include <algorithm>

const sf::Vector2f gridOffset({100.f, 200.f});
const sf::Vector2f inventoryOffset({540.f, 200.f});

std::vector<PlacedItem> placedItems;

GameGUI::GameGUI()
    : inventory(5, 3, sf::Color::Transparent, sf::Color::White),
      manager(40.f, {100.f, 200.f}),
      grid(10, std::vector<bool>(10, false)),
      cellSize(40.0f),
      title(nullptr),
      backText(nullptr),
      endText(nullptr),
      descriptionText(nullptr),
      currentLevel(5),
      completedTasksCount(0),
      activeFigureIndex(0)
{
    fontLoaded = font.openFromFile("Tokushupikuseru-Regular.otf");
    
    backTexture.loadFromFile("BtnBackground.jpg");
    endTexture.loadFromFile("BtnBackground.jpg");
    delTexture.loadFromFile("BtnBackground.jpg");   // добавил

    descriptionText = std::make_unique<sf::Text>(font, "", 24);

    std::ifstream taskFile("../data/static/BD/tasks.json");
    if (taskFile.is_open()) 
    {
        json tasksJson;
        taskFile >> tasksJson;
        for (auto& [key, value] : tasksJson.items())
        {
           int level = std::stoi(key);
           std::vector<Task> tasks;
           for (const auto& task : value)
           {
            Task t;
            t.type = task.value("type", "");          // добавил поле type
            t.target = task.contains("target") ? task["target"].get<float>() : 0.0f;
            t.description = task["description"];
            t.completed = false;
            tasks.push_back(t);
           }
           allTasks.push_back(tasks);
        }
    }

    loadItems();
    loadTasks();
    loadLevel(currentLevel);
    loadLevelTasks(currentLevel);
    createDefaultFigure();
    updateItemDescription();   // новое
    updateTasksStatus();       // новое

    float winWidth = 1600.f;
    float winHeight = 900.f;

    if (fontLoaded)
    {
        title = std::make_unique<sf::Text>(font, "Game Screen", 120);
        title->setPosition({600.f, -50.f});
        title->setFillColor(sf::Color::White);

        backText = std::make_unique<sf::Text>(font, "Back", 60);
        backText->setPosition({130.f, 790.f});
        backText->setFillColor(sf::Color::Black);

        endText = std::make_unique<sf::Text>(font, "End", 60);
        endText->setPosition({1400.f, 790.f});
        endText->setFillColor(sf::Color::Black);

        delText = std::make_unique<sf::Text>(font, "Delete Item", 60);  // добавил
        delText->setPosition({610.f, 490.f});
        delText->setFillColor(sf::Color::Black);

        descriptionText = std::make_unique<sf::Text>(font, "", 60);
        descriptionText->setFillColor(sf::Color::White);
    }

    backBtn.setSize({300, 80});
    backBtn.setPosition({20.f, 800.f});
    backBtn.setTexture(&backTexture);

    endBtn.setSize({300, 80});
    endBtn.setPosition({1280.f, 800.f});
    endBtn.setTexture(&endTexture);

    delBtn.setSize({300, 80});                        // добавил
    delBtn.setPosition({545.f, 500.f});
    delBtn.setTexture(&delTexture);

    updateItemDescription();
    updateTasksStatus();
}

GameGUI::~GameGUI() = default;

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
                    if (inventory.placeItemWithColor(player, cells, player.color)) {
                        std::cout << "Item placed in inventory!" << cells[0].x << cells[0].y << std::endl;
                        
                        // Сохраняем информацию о размещённом предмете в placedItems
                        int itemId = -1;
                        for (const auto& [id, data] : itemsData) {
                            if (data.contains("color")) {
                                auto colorArray = data["color"];
                                if (colorArray.size() == 3) {
                                    sf::Color itemColor(
                                        colorArray[0].get<int>(),
                                        colorArray[1].get<int>(),
                                        colorArray[2].get<int>()
                                    );
                                    if (player.color == itemColor) {
                                        itemId = id;
                                        break;
                                    }
                                }
                            }
                        }
                        if (itemId != -1) {
                            PlacedItem p;
                            p.id = itemId;
                            p.name = itemsData[itemId].value("name", "No name");
                            p.price = itemsData[itemId].value("price", 0);
                            p.category = itemsData[itemId].value("affiliation", "Unknown");
                            p.cells = cells;
                            placedItems.push_back(p);
                        }
                        
                        // Удаляем активную фигуру
                        if (!availableFigures.empty() && activeFigureIndex < availableFigures.size()) {
                            availableFigures.erase(availableFigures.begin() + activeFigureIndex);
                        }
                        
                        // Обновляем активную фигуру
                        if (!availableFigures.empty()) {
                            activeFigureIndex = 0;
                            player = availableFigures[0];
                            player.position = {0, 0};
                            updateItemDescription();
                            updateTasksStatus();  // обновляем задания
                        } else {
                            lastCompletedTasks = completedTasksCount;
                            collectStats();       // собираем статистику перед переходом
                            currentAppStatus = AppStatus::GAMERESULTS;
                            dragState.reset();
                            return;
                        }
                        dragState.reset();
                        return;

                    }
                } 
                else 
                {
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

// ============================================
// ЗАГРУЗКА УРОВНЕЙ И ЗАДАНИЙ
// ============================================

void GameGUI::loadLevelTasks(int level)
{
    if (level >= 1 && level <= (int)allTasks.size()) {
        currentTasks = allTasks[level - 1];
        for (auto& task : currentTasks)
            task.completed = false;
    }
}

void GameGUI::updateTasksStatus()
{
    int totalCost = 0;
    int usedCells = 0;
    int itemCount = 0;
    std::set<std::string> categories;

    for (const auto& item : placedItems) {
        totalCost += item.price;
        usedCells += item.cells.size();
        itemCount++;
        categories.insert(item.category);
    }

    int completed = 0;
    for (auto& task : currentTasks) {
        bool isCompleted = false;
        if (task.type == "min_cost") {
            isCompleted = (totalCost >= task.target);
        } else if (task.type == "min_cells") {
            isCompleted = (usedCells >= task.target);
        } else if (task.type == "min_efficiency") {
            if (usedCells > 0) {
                float efficiency = static_cast<float>(totalCost) / usedCells;
                isCompleted = (efficiency >= task.target);
            } else {
                isCompleted = false;
            }
        } else if (task.type == "all_categories") {
            isCompleted = (categories.size() == 3);
        } else if (task.type == "use_all") {
            isCompleted = (itemCount == totalFiguresInLevel);
        } else if (task.type == "min_items") {
            isCompleted = (itemCount >= task.target);
        } else if (task.type == "max_items") {
            isCompleted = (itemCount <= task.target);
        } else if (task.type == "max_cells") {
            isCompleted = (usedCells <= task.target);
        } else {
            isCompleted = false;
        }
        task.completed = isCompleted;
        if (isCompleted) completed++;
    }
    completedTasksCount = completed;
}

void GameGUI::loadItems()
{
    std::ifstream file("../data/static/BD/item.json");
    if (!file.is_open()) file.open("data/static/BD/item.json");
    if (file.is_open()) {
        json data;
        file >> data;
        for (auto& [key, value] : data.items()) {
            int id = std::stoi(key);
            itemsData[id] = value;
        }
    }
}

void GameGUI::loadTasks()
{
    // Загружается в конструкторе
}

void GameGUI::createDefaultFigure()
{
    if (availableFigures.empty() && itemsData.find(3) != itemsData.end())
    {
        Tetromino fig = manager.createFromJSON(itemsData[3]);
        fig.position = {0, 0};
        availableFigures.push_back(fig);
        player = availableFigures[0];
        updateItemDescription();
        updateTasksStatus();
    }
}

void GameGUI::loadLevel(int levelID)
{
    currentLevelId = levelID;
    availableFigures.clear();

    std::ifstream file("../data/static/BD/levels.json");
    if (file.is_open())
    {
        json levels;
        file >> levels;
        std::string key = std::to_string(levelID);
        if (levels.contains(key))
        {
            availableFigures.clear();
            float startX = 0.f;
            float yPos = 0.f;

            for (int id : levels[key])
            {
                if (itemsData.find(id) != itemsData.end())
                {
                    Tetromino fig = manager.createFromJSON(itemsData[id]);
                    fig.position = {static_cast<int>(startX/cellSize),
                                    static_cast<int>(yPos/cellSize)};
                    availableFigures.push_back(fig);
                    startX += 50.f;
                }
            }
            totalFiguresInLevel = availableFigures.size();  // важно для use_all
            if (!availableFigures.empty())
            {    
                player = availableFigures[0];
                updateItemDescription();
                updateTasksStatus();
            }
        }
    }
    loadLevelTasks(levelID);
}

// ============================================
// ОТОБРАЖЕНИЕ ИНФОРМАЦИИ О ПРЕДМЕТЕ
// ============================================

void GameGUI::updateItemDescription() {
    std::string info;
    for (const auto& [id, data] : itemsData) {
        if (data.contains("color")) {
            auto colorArray = data["color"];
            if (colorArray.size() == 3) {
                sf::Color itemColor(
                    colorArray[0].get<int>(),
                    colorArray[1].get<int>(),
                    colorArray[2].get<int>()
                );
                if (player.color == itemColor) {
                    info += "Name: " + data.value("name", "No name") + "\n";
                    info += "Price: " + std::to_string(data.value("price", 0)) + "\n";
                    info += "Category: " + data.value("affiliation", "Unknown") + "\n";
                    info += "Size: " + std::to_string(player.shape.size()) + " cells";
                    break;
                }
            }
        }
    }
    if (info.empty()) {
        info = "Unknown item";
    }
    descriptionText->setString(info);
    descriptionLines.clear();
    std::stringstream ss(info);
    std::string line;
    while (std::getline(ss, line, '\n')) {
        descriptionLines.push_back(line);
    }
}

// ============================================
// СБОР СТАТИСТИКИ ДЛЯ ЭКРАНА РЕЗУЛЬТАТОВ
// ============================================

void GameGUI::collectStats() {
    int totalCost = 0;
    int usedCells = 0;
    for (const auto& item : placedItems) {
        totalCost += item.price;
        usedCells += item.cells.size();
    }
    GameStats::totalCost = totalCost;
    GameStats::itemCount = placedItems.size();
    GameStats::usedCells = usedCells;
    GameStats::totalCells = inventory.getWidth() * inventory.getHeight();
    GameStats::completedTasksCount = completedTasksCount;
    GameStats::categoryCount.clear();
    for (const auto& item : placedItems) {
        GameStats::categoryCount[item.category]++;
    }
    GameStats::currentLevel = currentLevel;
}

// ============================================
// УДАЛЕНИЕ ПРЕДМЕТА ИЗ placedItems
// ============================================

void GameGUI::removePlacedItem(const std::vector<sf::Vector2i>& cells) {
    auto it = std::find_if(placedItems.begin(), placedItems.end(),
        [&](const PlacedItem& p) {
            if (p.cells.size() != cells.size()) return false;
            for (const auto& c : cells) {
                if (std::find(p.cells.begin(), p.cells.end(), c) == p.cells.end())
                    return false;
            }
            return true;
        });
    if (it != placedItems.end()) {
        placedItems.erase(it);
    }
}

// ============================================
// ОБРАБОТКА СОБЫТИЙ
// ============================================

void GameGUI::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    // 1. Сначала обрабатываем drag-and-drop события
    if (event.is<sf::Event::MouseButtonPressed>()) {
        startDrag(event);
    }
    
    if (event.is<sf::Event::MouseMoved>()) {
        updateDrag(event);
    }
    
    if (event.is<sf::Event::MouseButtonReleased>()) {
        endDrag(event);
    }

    // 2. Обработка клавиатуры (только если не в режиме перетаскивания)
    if (!dragState.isDragging) {
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
                            if (inventory.placeItemWithColor(player, cells, player.color)) {
                                // Сохраняем в placedItems
                                int itemId = -1;
                                for (const auto& [id, data] : itemsData) {
                                    if (data.contains("color")) {
                                        auto colorArray = data["color"];
                                        if (colorArray.size() == 3) {
                                            sf::Color itemColor(
                                                colorArray[0].get<int>(),
                                                colorArray[1].get<int>(),
                                                colorArray[2].get<int>()
                                            );
                                            if (player.color == itemColor) {
                                                itemId = id;
                                                break;
                                            }
                                        }
                                    }
                                }
                                if (itemId != -1) {
                                    PlacedItem p;
                                    p.id = itemId;
                                    p.name = itemsData[itemId].value("name", "No name");
                                    p.price = itemsData[itemId].value("price", 0);
                                    p.category = itemsData[itemId].value("affiliation", "Unknown");
                                    p.cells = cells;
                                    placedItems.push_back(p);
                                }
                                // Удаляем активную фигуру по индексу
                                if (!availableFigures.empty() && activeFigureIndex < availableFigures.size()) {
                                    availableFigures.erase(availableFigures.begin() + activeFigureIndex);
                                }
                                // Обновляем активную фигуру
                                if (!availableFigures.empty()) {
                                    activeFigureIndex = 0; // берём первую из оставшихся
                                    player = availableFigures[0];
                                    player.position = {0, 0};
                                    updateItemDescription();
                                    updateTasksStatus();
                                } else {
                                    // Все фигуры размещены
                                    lastCompletedTasks = completedTasksCount;
                                    collectStats();
                                    currentAppStatus = AppStatus::GAMERESULTS;
                                    return;
                                }
                            }
                        }
                        break;
                    }
                    case sf::Keyboard::Key::Escape:
                        cancelDrag(); // Отменяем drag при Escape
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
    // 3. Обработка кнопок мыши (только если не в режиме перетаскивания)
    if (!dragState.isDragging) {
        if (event.is<sf::Event::MouseButtonPressed>()) {
            auto mouse = event.getIf<sf::Event::MouseButtonPressed>();
            if (mouse->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos(static_cast<float>(mouse->position.x), 
                                     static_cast<float>(mouse->position.y));
                if (backBtn.getGlobalBounds().contains(mousePos)) {
                    currentAppStatus = AppStatus::MAINMENU;
                }
                if (endBtn.getGlobalBounds().contains(mousePos)) 
                {
                    collectStats();
                    lastCompletedTasks = completedTasksCount;
                    currentAppStatus = AppStatus::GAMERESULTS;
                }
                if (delBtn.getGlobalBounds().contains(mousePos)) {
                    // Удаляем активную фигуру
                    if (!availableFigures.empty() && activeFigureIndex < availableFigures.size()) {
                        auto cellsToRemove = manager.getAbsoluteCells(player);
                        removePlacedItem(cellsToRemove);
                        availableFigures.erase(availableFigures.begin() + activeFigureIndex);
                        if (!availableFigures.empty()) {
                            activeFigureIndex = activeFigureIndex % availableFigures.size();
                            player = availableFigures[activeFigureIndex];
                            player.position = {0, 0};
                            updateItemDescription();
                            updateTasksStatus();
                        } else {
                            collectStats();
                            lastCompletedTasks = completedTasksCount;
                            currentAppStatus = AppStatus::GAMERESULTS;
                            dragState.reset();
                            return;
                        }
                    }
                }
            }
            
        }
    }
}

// ============================================
// ОТРИСОВКА
// ============================================

void GameGUI::render(sf::RenderWindow& window)
{
    // Рисуем сетку
    for (int y = 0; y < 5; ++y) {
        for (int x = 0; x < 5; ++x) {
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
    
    // Рисуем инвентарь
    inventory.draw(window, inventoryOffset, cellSize);
    
    // Рисуем фигуру с эффектом перетаскивания
    if (dragState.isDragging) {
        // Рисуем с подсветкой
        manager.drawDragging(window, player, sf::Color::Yellow, 3.0f);
        
        // Показываем превью позиции в инвентаре
        sf::Vector2f mousePos = lastMousePos;
        if (isMouseOnInventory(mousePos)) {
            // Рисуем прозрачную версию в позиции инвентаря
            Tetromino preview = player;
            sf::Vector2i invGridPos = manager.pixelToGrid(
                sf::Vector2f(mousePos.x - inventoryOffset.x, 
                            mousePos.y - inventoryOffset.y)
            );
            preview.position = invGridPos;
            
            auto cells = manager.getAbsoluteCells(preview);
            if (inventory.ValidInInventory(cells)) {
                // Зеленая подсветка - валидная позиция
                manager.drawWithAlpha(window, preview, 100);
            } else {
                // Красная подсветка - невалидная позиция
                sf::Color redColor = sf::Color::Red;
                redColor.a = 100;
                // Можно нарисовать красную обводку
            }
        }
    } 
    else 
    {
        // Обычная отрисовка
        manager.draw(window, player);
    }

    // Отрисовка описания предмета (ранее была отдельная переменная descriptionLines)
    if (!descriptionLines.empty())
    {
        float lineHeight = 30.f;
        float fontSize = 60.f;
        float startX = 650.f;
        float startY = 650.f;
        for (size_t i = 0; i < descriptionLines.size(); ++i) 
        {
            sf::Text lineText(font, descriptionLines[i], fontSize);
            lineText.setPosition(sf::Vector2f(startX, startY + i * lineHeight));
            lineText.setFillColor(sf::Color::White);
            window.draw(lineText);
        }
    }

    if (fontLoaded) 
    {
        float x = 900.f;
        float y = 140.f;
        sf::Text taskTitle(font, "Tasks:", 90);
        taskTitle.setPosition({x, y});
        taskTitle.setFillColor(sf::Color::White);
        window.draw(taskTitle);
        y += 90;

        for (const auto& task : currentTasks) {
            sf::Text taskText(font, task.description, 50);
            taskText.setPosition({x, y});
            taskText.setFillColor(task.completed ? sf::Color::Green : sf::Color::White);
            window.draw(taskText);
            y += 40;
        }
    }
    
    window.draw(backBtn);
    window.draw(endBtn);
    window.draw(delBtn);
    if (fontLoaded) {
        if (title) window.draw(*title);
        if (backText) window.draw(*backText);
        if (endText) window.draw(*endText);
        if (delText) window.draw(*delText);
    }
}