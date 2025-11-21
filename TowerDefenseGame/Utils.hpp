#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace Utils {
// 計算向量長度
inline float length(const sf::Vector2f& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

// 計算兩點距離 (用於判定塔的攻擊範圍)
inline float distance(const sf::Vector2f& a, const sf::Vector2f& b) {
    return length(a - b);
}

// 向量正規化 (用於計算移動方向)
inline sf::Vector2f normalize(const sf::Vector2f& v) {
    float len = length(v);
    if (len == 0) return {0.f, 0.f};
    return v / len;
}
}  // namespace Utils