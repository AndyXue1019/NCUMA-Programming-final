#include "Map.hpp"

Map::Map() {
    initGrid();
}

void Map::initGrid() {
    // 建立垂直線
    for (int i = 0; i <= Config::COLUMNS; ++i) {
        float x = i * Config::GRID_SIZE;
        m_gridLines.push_back(sf::Vertex{sf::Vector2f(x, 0.f), sf::Color(50, 50, 50)});
        m_gridLines.push_back(sf::Vertex{sf::Vector2f(x, static_cast<float>(Config::WINDOW_HEIGHT)), sf::Color(50, 50, 50)});
    }

    // 建立水平線
    for (int i = 0; i <= Config::ROWS; ++i) {
        float y = i * Config::GRID_SIZE;
        m_gridLines.push_back(sf::Vertex{sf::Vector2f(0.f, y), sf::Color(50, 50, 50)});
        m_gridLines.push_back(sf::Vertex{sf::Vector2f(static_cast<float>(Config::WINDOW_WIDTH), y), sf::Color(50, 50, 50)});
    }
}

void Map::draw(sf::RenderWindow& window) {
    // SFML 3 繪製 Vertex Array 的方式
    window.draw(m_gridLines.data(), m_gridLines.size(), sf::PrimitiveType::Lines);
}