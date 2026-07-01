#include <SFML/Graphics.hpp>
#include <memory>

class MainMenu
{
public:
    MainMenu();
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    void render(sf::RenderWindow& window);\

private:
    sf::Font font;
    bool fontLoaded;
    std::unique_ptr<sf::Text> title;
    std::unique_ptr<sf::Text> startText;
    std::unique_ptr<sf::Text> exitText;
    sf::RectangleShape startBtn;
    sf::RectangleShape exitBtn;
    sf::Texture startTexture;
    sf::Texture exitTexture;
};