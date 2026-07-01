#include <SFML/Graphics.hpp>
#include <vector>
#include "../object/Tetramino.hpp"
#include "../object/Inventory.hpp"

class GameGUI
{
public:
    GameGUI();
    ~GameGUI();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void render(sf::RenderWindow& window);

private:
    Inventory inventory;

    sf::Font font;
    bool fontLoaded;
    std::unique_ptr<sf::Text> title;
    std::unique_ptr<sf::Text> backText;
    std::unique_ptr<sf::Text> endText;
    
    sf::RectangleShape backBtn;
    sf::RectangleShape endBtn;

    TetrominoManager manager;
    Tetromino player;
    std::vector<std::vector<bool>> grid;
    float cellSize = 40.0;
    json data;

     // ===== НОВОЕ: Drag-and-drop состояние =====
    DragState dragState;
    sf::Vector2f lastMousePos;
    
    // ===== НОВОЕ: Методы drag-and-drop =====
    void startDrag(const sf::Event& event);
    void updateDrag(const sf::Event& event);
    void endDrag(const sf::Event& event);
    void cancelDrag();
    bool isMouseOnInventory(sf::Vector2f mousePos) const;
};