#pragma once
#include <SFML/Graphics.hpp>
#include <string>

enum class AccessoryType {
    // updating
    None,
    FireGem,
    IceGem,
    StormGem,
    ExplosiveGem
};

struct Accessory {
    AccessoryType type;
    std::string name;
    std::string description;
    sf::Color color; // 暫時用顏色代表圖示
    int price; //寶石售價
};

namespace AccessoryData {
    inline Accessory get(AccessoryType type) {
        switch (type) {
        case AccessoryType::FireGem:
            return {
                type,
                "Fire Gem",
                "Tower attacks have chance to\nburn enemies (3% HP/sec).",
                sf::Color(255, 69, 0), // Red-Orange
                10
            };
        case AccessoryType::IceGem:
            return { 
                type, 
                "Ice Gem", 
                "3% chance to freeze\nenemies for 1s.", 
                sf::Color(0, 255, 255), 
                15 
            };

        case AccessoryType::StormGem:
            return { 
                type, 
                "Storm Gem", 
                "All towers attack\nspeed +150%.", 
                sf::Color(255, 255, 0), 
                20 
            };

        case AccessoryType::ExplosiveGem:
            return { 
                type, 
                "Blast Gem", 
                "Chance to deal 200%\nAOE damage.", 
                sf::Color(139, 0, 0),
                20 
            };
        default:
            return { AccessoryType::None, "", "", sf::Color::White, 0};
        }
    }
}