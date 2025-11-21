#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

#include "Config.hpp"

class Map {
   public:
    Map();
    void draw(sf::RenderWindow& window);

   private:
    // 用於視覺化網格線
    std::vector<sf::Vertex> m_gridLines;

    // 初始化網格
    void initGrid();
};