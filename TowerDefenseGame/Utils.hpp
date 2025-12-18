#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <random>

namespace Utils {
// 全域隨機數生成器 (用於各種機率判定)
inline std::random_device rd;
inline std::mt19937 m_rnd(rd());
inline std::uniform_int_distribution<int> m_dist{0, 99};
// 取得 0-99 的隨機整數 (相當於 rand() % 100)
inline int m_rand() { return m_dist(m_rnd); }

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