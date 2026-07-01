#include <SFML/Graphics.hpp>
#include <string>
#include <memory>

class GameResults
{
public:
    GameResults();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void render(sf::RenderWindow& window);
private:
    sf::Font font;
    bool fontLoaded;

    std::unique_ptr<sf::Text> title;
    std::unique_ptr<sf::Text> totalCostText;
    std::unique_ptr<sf::Text> itemCountText;
    std::unique_ptr<sf::Text> usedCellsText;

    sf::RectangleShape tryAgainBtn;
    sf::RectangleShape mainMenuBtn;
    std::unique_ptr<sf::Text> tryAgainText;
    std::unique_ptr<sf::Text> mainMenuText;
};