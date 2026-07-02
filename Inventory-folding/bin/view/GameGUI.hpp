#include <SFML/Graphics.hpp>
#include <vector>
#include "../object/Tetramino.hpp"
#include "../object/Inventory.hpp"

using json = nlohmann::json;

struct DragStateGUI {
    bool isDragging = false;
    sf::Vector2f offset;
    Tetromino* draggedTetromino = nullptr;
    int figureIndex = -1;
    sf::Vector2i startGridPos;
    sf::Vector2f dragStartMouse;
    sf::Vector2f mouseOffset;
    sf::Vector2i grabbedCell;
    
    void reset() {
        isDragging = false;
        figureIndex = -1;
    }
};

struct Task {
    std::string type;
    float target;
    std::string description;
    bool completed;
};

struct PlacedItem 
{
    int id;
    std::string name;
    int price;
    std::string category;
    std::vector<sf::Vector2i> cells;
};

class GameGUI
{
public:
    GameGUI();
    ~GameGUI();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void render(sf::RenderWindow& window);
    void loadLevelTasks(int level);
    void loadItems();
    void loadTasks();
    void updateTasksStatus();
    void createDefaultFigure();
    void fixPlayer();
    void loadLevel(int levelId);
    void updateItemDescription();
    void collectStats();
    void removePlacedItem(const std::vector<sf::Vector2i>& cells);
private:
    Inventory inventory;

    sf::Font font;
    bool fontLoaded;
    std::unique_ptr<sf::Text> title;
    std::unique_ptr<sf::Text> backText;
    std::unique_ptr<sf::Text> endText;
    std::unique_ptr<sf::Text> delText;
    std::unique_ptr<sf::Text> descriptionText;
    std::vector<std::string> descriptionLines;
    
    sf::RectangleShape backBtn;
    sf::RectangleShape endBtn;
    sf::RectangleShape delBtn;
    sf::Texture backTexture;
    sf::Texture endTexture;
    sf::Texture delTexture;

    std::map<int, json> itemsData;

    std::vector<std::vector<Task>> allTasks;
    std::vector<Task> currentTasks;
    int currentLevel;
    int completedTasksCount;    
    int totalFiguresInLevel;
    std::vector<PlacedItem> placedItems;

    TetrominoManager manager;
    Tetromino player;
    std::vector<std::vector<bool>> grid;
    float cellSize = 40.0;
    json data;
    
    std::vector<Tetromino> availableFigures;
    int activeFigureIndex = 0;
    int currentFigureIndex;
    int currentLevelId;

     // ===== НОВОЕ: Drag-and-drop состояние =====
    DragStateGUI dragState;
    sf::Vector2f lastMousePos;
    
    // ===== НОВОЕ: Методы drag-and-drop =====
    void startDrag(const sf::Event& event);
    void updateDrag(const sf::Event& event);
    void endDrag(const sf::Event& event);
    void cancelDrag();
    bool isMouseOnInventory(sf::Vector2f mousePos) const;
};