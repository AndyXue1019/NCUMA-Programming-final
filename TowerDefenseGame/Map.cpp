#include "Map.hpp"

#include <algorithm>
#include <cmath>

Map::Map(sf::Vector2u gridSize, float tileSize)
    : m_gridSize(gridSize),
    m_tileSize(tileSize),
    m_heat(gridSize.x* gridSize.y, 0.f),
    m_vertices(sf::PrimitiveType::Triangles) {
    buildVertices();
}

void Map::buildVertices() {
    m_vertices.clear();
    m_vertices.resize(static_cast<std::size_t>(m_gridSize.x) * m_gridSize.y * 6); // 6 verts per tile (2 triangles)
    std::size_t idx = 0;
    for (unsigned y = 0; y < m_gridSize.y; ++y) {
        for (unsigned x = 0; x < m_gridSize.x; ++x) {
            float left = x * m_tileSize;
            float top = y * m_tileSize;
            float right = left + m_tileSize;
            float bottom = top + m_tileSize;

            // Triangle 1
            m_vertices[idx++].position = { left, top };
            m_vertices[idx++].position = { right, top };
            m_vertices[idx++].position = { right, bottom };

            // Triangle 2
            m_vertices[idx++].position = { left, top };
            m_vertices[idx++].position = { right, bottom };
            m_vertices[idx++].position = { left, bottom };
        }
    }
    // initial colors
    updateColors();
}

sf::Vector2u Map::worldToTile(const sf::Vector2f& worldPos) const {
    int tx = static_cast<int>(std::floor(worldPos.x / m_tileSize));
    int ty = static_cast<int>(std::floor(worldPos.y / m_tileSize));
    tx = std::clamp(tx, 0, static_cast<int>(m_gridSize.x) - 1);
    ty = std::clamp(ty, 0, static_cast<int>(m_gridSize.y) - 1);
    return { static_cast<unsigned>(tx), static_cast<unsigned>(ty) };
}

void Map::addPosition(const sf::Vector2f& worldPos, float weight) {
    auto t = worldToTile(worldPos);
    std::size_t index = static_cast<std::size_t>(t.y * m_gridSize.x + t.x);
    m_heat[index] += weight;
    if (m_heat[index] > m_maxHeat) m_maxHeat = m_heat[index];
    updateColors();
}

void Map::addPath(const std::vector<sf::Vector2f>& worldPath) {
    if (worldPath.empty()) return;
    // 在點之間插值，每隔一小段取樣加入
    for (std::size_t i = 0; i + 1 < worldPath.size(); ++i) {
        sf::Vector2f a = worldPath[i];
        sf::Vector2f b = worldPath[i + 1];
        float dx = b.x - a.x;
        float dy = b.y - a.y;
        float dist = std::hypot(dx, dy);
        int steps = std::max(1, static_cast<int>(dist / (m_tileSize * 0.5f)));
        for (int s = 0; s <= steps; ++s) {
            float t = steps ? (static_cast<float>(s) / steps) : 0.f;
            sf::Vector2f p{ a.x + dx * t, a.y + dy * t };
            addPosition(p, 1.f);
        }
    }
    // 最後一個點也加入（若只有一個點）
    if (worldPath.size() == 1) addPosition(worldPath[0], 1.f);
}

void Map::decay(float factor) {
    factor = std::clamp(factor, 0.f, 1.f);
    m_maxHeat = 0.f;
    for (auto& v : m_heat) {
        v *= factor;
        if (v > m_maxHeat) m_maxHeat = v;
    }
    if (m_maxHeat < 1e-6f) m_maxHeat = 1.f; // avoid div0
    updateColors();
}

void Map::clear() {
    std::fill(m_heat.begin(), m_heat.end(), 0.f);
    m_maxHeat = 1.f;
    updateColors();
}

sf::Color Map::colorForValue(float normalized) const {
    // normalized in [0,1];
    normalized = std::clamp(normalized, 0.f, 1.f);

    // 0 熱度保留透明，讓背景（Game::render 清除為黑色）可以顯示為黑色
    if (normalized <= 1e-6f) {
        return sf::Color(0, 0, 0, 0);
    }

    // 只要有熱度就顯示黃色 (路徑)
    // 使用 alpha=200 作為半透明效果，若想完全不透明可改為 255
    return sf::Color(255, 255, 0, 200);
}

void Map::updateColors() {
    // 找最大值（已由 addPosition/decay 維護，但保險起見再計算一次）
    float maxv = 0.f;
    for (float v : m_heat) if (v > maxv) maxv = v;
    if (maxv <= 0.f) maxv = 1.f;
    // 為每一個 tile 填色（對應到 6 vertices）
    std::size_t vi = 0;
    for (unsigned y = 0; y < m_gridSize.y; ++y) {
        for (unsigned x = 0; x < m_gridSize.x; ++x) {
            std::size_t idx = static_cast<std::size_t>(y * m_gridSize.x + x);
            float normalized = m_heat[idx] / maxv;
            sf::Color c = colorForValue(normalized);
            // 設定接下來 6 個 vertices 的顏色
            for (int k = 0; k < 6; ++k) {
                m_vertices[vi++].color = c;
            }
        }
    }
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(m_vertices, states);
}