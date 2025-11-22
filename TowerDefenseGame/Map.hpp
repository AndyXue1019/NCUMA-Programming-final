#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Map : public sf::Drawable {
public:
    Map() = default;
    // gridSize: {cols, rows}, tileSize: 像素
    Map(sf::Vector2u gridSize, float tileSize);

    // 將世界座標路徑（單位同視窗像素）加入熱度
    void addPath(const std::vector<sf::Vector2f>& worldPath);

    // 加單一點（例如每個敵人每幀呼叫）
    void addPosition(const sf::Vector2f& worldPos, float weight = 1.f);

    // 每幀可呼叫來淡化先前統計 (0..1), 例如 0.99f
    void decay(float factor);

    // 重置所有
    void clear();

    // (選用) 取得格子座標
    sf::Vector2u worldToTile(const sf::Vector2f& worldPos) const;

    // 取得目前最大熱度（可用於調整顏色尺度）
    float maxHeat() const { return m_maxHeat; }

    // Drawable override
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
    void buildVertices();
    void updateColors(); // 用 m_heat->m_vertices.colors
    sf::Color colorForValue(float normalized) const; // 0..1

    

    sf::Vector2u m_gridSize;
    float m_tileSize;
    std::vector<float> m_heat; // size = cols*rows
    sf::VertexArray m_vertices; // PrimitiveType::Triangles, 每格 6 個 vertices
    float m_maxHeat{ 1.f };
};