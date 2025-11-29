#pragma once
#include <string>
#include <SFML/Graphics.hpp>

enum class AccessoryType {
    // updating
    None,
    FireGem,
};

struct Accessory {
    AccessoryType type;
    std::string name;
    std::string description;
    sf::Color color; // 暫時用顏色代表圖示
};

namespace AccessoryData {
    inline Accessory get(AccessoryType type) {
        switch (type) {
        case AccessoryType::FireGem:
            return {
                type,
                "Fire Gem",
                "Tower attacks have chance to\nburn enemies (3% HP/sec).",
                sf::Color(255, 69, 0) // Red-Orange
            };
        default:
            return { AccessoryType::None, "", "", sf::Color::White };
        }
    }
}