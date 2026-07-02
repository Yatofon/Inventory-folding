#pragma once
#include <map>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

// Структура для хранения полной статистики рекорда
struct RecordData {
    double score = 0.0;
    int totalCost = 0;
    int itemCount = 0;
    int usedCells = 0;
    int totalCells = 0;
    int completedTasks = 0;
};

namespace GameStats {
    // Текущая статистика (заполняется из GameGUI)
    inline int totalCost = 0;
    inline int itemCount = 0;
    inline int usedCells = 0;
    inline int totalCells = 15;
    inline int completedTasksCount = 0;
    inline std::map<std::string, int> categoryCount;
    inline int currentLevel = 1;

    // Хранилище рекордов по уровням
    inline std::map<int, RecordData> bestRecords;
}

// Загрузка рекордов из файла
inline void loadRecords() {
    std::ifstream file("data/static/BD/records.json");
    if (!file.is_open()) {
        std::cerr << "No records file found, starting fresh." << std::endl;
        return;
    }
    json j;
    file >> j;
    for (auto& [key, value] : j.items()) {
        int level = std::stoi(key);
        RecordData rec;
        rec.score = value["score"].get<double>();
        rec.totalCost = value["totalCost"].get<int>();
        rec.itemCount = value["itemCount"].get<int>();
        rec.usedCells = value["usedCells"].get<int>();
        rec.totalCells = value["totalCells"].get<int>();
        rec.completedTasks = value["completedTasks"].get<int>();
        GameStats::bestRecords[level] = rec;
    }
    std::cout << "Records loaded." << std::endl;
}

// Сохранение рекордов в файл
inline void saveRecords() {
    std::ofstream file("../data/static/BD/records.json");
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