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
};