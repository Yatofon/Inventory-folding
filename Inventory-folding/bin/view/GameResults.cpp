#include "GameResults.hpp"
#include "AppStatus.hpp"
#include "GameStats.hpp"
#include <SFML/Graphics.hpp>
#include <map>
#include <cmath>

GameResults::GameResults()
{
    fontLoaded = font.openFromFile("Tokushupikuseru-Regular.otf");
    tryAgainTexture.loadFromFile("BtnBackground.jpg");
    mainMenuTexture.loadFromFile("BtnBackground.jpg");

    if (fontLoaded) 
    {
        title = std::make_unique<sf::Text>(font, "Results", 120);
        title->setPosition({100.f, -50.f});
        title->setFillColor(sf::Color::White);

        totalCostText = std::make_unique<sf::Text>(font, "Total coas is ", 60);
        totalCostText->setPosition({50.f, 100.f});
        totalCostText->setFillColor(sf::Color::White);

        itemCountText = std::make_unique<sf::Text>(font, "Total cells is ", 60);
        itemCountText->setPosition({50.f, 180.f});
        itemCountText->setFillColor(sf::Color::White);

        usedCellsText = std::make_unique<sf::Text>(font, "Used cells ", 60);
        usedCellsText->setPosition({50.f, 260.f});
        usedCellsText->setFillColor(sf::Color::White);

        tryAgainText = std::make_unique<sf::Text>(font, "Try again", 60);
        tryAgainText->setPosition({345.f, 790.f});
        tryAgainText->setFillColor(sf::Color::Black);

        mainMenuText = std::make_unique<sf::Text>(font, "Main menu", 60);
        mainMenuText->setPosition({1110.f, 790.f});
        mainMenuText->setFillColor(sf::Color::Black);

        completedTasksText = std::make_unique<sf::Text>(font, "Completed tasks " + std::to_string(GameStats::completedTasksCount), 60);
        completedTasksText->setPosition({50.f, 340.f});
        completedTasksText->setFillColor(sf::Color::White);
    }

    tryAgainBtn.setSize({300, 80});
    tryAgainBtn.setPosition({270.f, 800.f});
    tryAgainBtn.setTexture(&tryAgainTexture);

    mainMenuBtn.setSize({300, 80});
    mainMenuBtn.setPosition({1030.f, 800.f});
    mainMenuBtn.setTexture(&mainMenuTexture);
}

void GameResults::handleEvent(const sf::Event& event, sf::RenderWindow& window)
{
    if (event.is<sf::Event::MouseButtonPressed>())
    {
        auto mouse = event.getIf<sf::Event::MouseButtonPressed>();
        if (mouse->button == sf::Mouse::Button::Left)
        {
            sf::Vector2f mousePos(static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y));
            if (tryAgainBtn.getGlobalBounds().contains(mousePos))
                currentAppStatus = AppStatus::GAMEGUI;
            if (mainMenuBtn.getGlobalBounds().contains(mousePos)) {
                currentAppStatus = AppStatus::MAINMENU;
            }
        }
    }
}

void GameResults::render(sf::RenderWindow& window)
{
    if (fontLoaded && totalCostText && itemCountText && usedCellsText) 
    {
        totalCostText->setString("Total cost: " + std::to_string(GameStats::totalCost));
        itemCountText->setString("Items: " + std::to_string(GameStats::itemCount));
        usedCellsText->setString("Used cells: " + std::to_string(GameStats::usedCells) + "/" + std::to_string(GameStats::totalCells));
        completedTasksText->setString("Completed tasks: " + std::to_string(GameStats::completedTasksCount));
    }
    window.draw(tryAgainBtn);
    window.draw(mainMenuBtn);

    if (fontLoaded)
    {
        if (title) window.draw(*title);
        if (totalCostText) window.draw(*totalCostText);
        if (itemCountText) window.draw(*itemCountText);
        if (usedCellsText) window.draw(*usedCellsText);
        if (tryAgainText) window.draw(*tryAgainText);
        if (mainMenuText) window.draw(*mainMenuText);
        if (completedTasksText) window.draw(*completedTasksText);
    }

    // ---- Рекорд ----
    double currentScore = 0.0;
    if (GameStats::usedCells > 0) {
        currentScore = static_cast<double>(GameStats::totalCost) * GameStats::totalCost / GameStats::usedCells;
    }
    int level = GameStats::currentLevel;
    bool isNewRecord = false;
    auto& best = GameStats::bestRecords[level];
    if (currentScore > best.score) {
        best.score = currentScore;
        best.totalCost = GameStats::totalCost;
        best.itemCount = GameStats::itemCount;
        best.usedCells = GameStats::usedCells;
        best.totalCells = GameStats::totalCells;
        best.completedTasks = GameStats::completedTasksCount;
        isNewRecord = true;
    }

    // Отображаем лучший счёт
    sf::Text recordText(font, "Best score: " + std::to_string(best.score), 120);
    recordText.setPosition(sf::Vector2f(800.f, -50.f));
    recordText.setFillColor(isNewRecord ? sf::Color::Green : sf::Color::White);
    window.draw(recordText);

    // ---- Статистика рекорда (по строкам) ----
    std::vector<std::string> lines = {
        "Cost: " + std::to_string(best.totalCost),
        "Items: " + std::to_string(best.itemCount),
        "Used cells: " + std::to_string(best.usedCells) + "/" + std::to_string(best.totalCells),
        "Tasks: " + std::to_string(best.completedTasks)
    };

    float startX = 800.f;
    float startY = 100.f;
    float lineHeight = 80.f;   // ← уменьши это число, чтобы строки были ближе
    int fontSize = 60;

    for (size_t i = 0; i < lines.size(); ++i) {
        sf::Text lineText(font, lines[i], fontSize);
        lineText.setPosition(sf::Vector2f(startX, startY + i * lineHeight));
        lineText.setFillColor(sf::Color::White);
        window.draw(lineText);
    }
    // ---- Круговая диаграмма категорий ----
    if (!GameStats::categoryCount.empty()) {
        float total = 0;
        for (const auto& [cat, count] : GameStats::categoryCount) {
            total += count;
        }
        float startAngle = 0.f;
        sf::Vector2f center(200.f, 600.f);
        float radius = 150.f;

        // Яркие цвета
        std::map<std::string, sf::Color> catColors = {
            {"construction", sf::Color(255, 200, 0)},   // золотой
            {"tools", sf::Color(0, 200, 255)},          // яркий голубой
            {"electronics", sf::Color(0, 255, 100)}     // яркий зелёный
        };

        int idx = 0;
        for (const auto& [cat, count] : GameStats::categoryCount) {
            if (count == 0) continue;
            float angle = (count / total) * 360.f;
            int segments = 30;
            sf::VertexArray sector(sf::PrimitiveType::TriangleFan, segments + 2);
            sector[0].position = center;
            sector[0].color = catColors[cat];

            for (int i = 0; i <= segments; ++i) {
                float rad = (startAngle + i * angle / segments) * 3.14159f / 180.f;
                sf::Vector2f point(
                    center.x + radius * std::cos(rad),
                    center.y + radius * std::sin(rad)
                );
                sector[i + 1].position = point;
                sector[i + 1].color = catColors[cat];
            }
            window.draw(sector);
            startAngle += angle;

            sf::Text legend(font, cat + ": " + std::to_string(count) + " (" + std::to_string((int)(count/total*100)) + "%)", 60);
            legend.setPosition(sf::Vector2f(center.x + radius + 20, 650.f + 30 * idx));
            legend.setFillColor(catColors[cat]);
            window.draw(legend);
            idx++;
        }
    }
    if (currentScore > best.score)
        saveRecords();
}

void saveRecordsToFile() {
    std::ofstream file("data/static/BD/records.json");
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open records.json for writing!" << std::endl;
        return;
    }
    json j;
    for (const auto& [level, rec] : GameStats::bestRecords) {
        json obj;
        obj["score"] = rec.score;
        obj["totalCost"] = rec.totalCost;
        obj["itemCount"] = rec.itemCount;
        obj["usedCells"] = rec.usedCells;
        obj["totalCells"] = rec.totalCells;
        obj["completedTasks"] = rec.completedTasks;
        j[std::to_string(level)] = obj;
    }
    file << j.dump(4);
    std::cout << "Records saved to data/static/BD/records.json" << std::endl;
}

