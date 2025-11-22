#pragma once
#include <SFML/Graphics/Color.hpp>
#include <map>
#include <string>

#include "Tower.hpp"  // TowerType enum

struct TowerInfo {
    std::string name;
    int price;
    int requiredLevel;  // 解鎖需要的玩家等級 (星級)
    sf::Color color;    // 用於 UI 圖示
};

namespace TowerData {
inline const std::map<TowerType, TowerInfo> INFO = {
    {TowerType::Basic, {"Basic", 50, 1, sf::Color::White}},
    {TowerType::Laser, {"Laser", 120, 1, sf::Color::Red}},
    {TowerType::Sniper, {"Sniper", 200, 2, sf::Color::Green}},
    {TowerType::Slow, {"Slow", 150, 2, sf::Color(0, 255, 255)}},  // Cyan
    {TowerType::Teleport, {"Teleport", 300, 3, sf::Color::Magenta}},
    {TowerType::SelfDestruct, {"Mine", 100, 1, sf::Color(50, 50, 50)}}};  // Dark Gray
}