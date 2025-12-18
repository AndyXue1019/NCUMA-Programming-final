#pragma once
#include <map>
#include <vector>

#include "Accessory.hpp"
#include "TowerData.hpp"

struct PlayerStats {
    int gold = 999999;
    int lives = 20;
    int level = 1;
    int experience = 0;
    int currentWave = 0;

    std::map<TowerType, int> inventory;

    // ¹¢«~¨t²Î
    std::vector<AccessoryType> accessories;
    AccessoryType activeAccessory = AccessoryType::None;

    // Extreme Gambler Event
    bool hasGambler = false;
    bool hasTriggeredGamblerEvent = false;

    void equipAccessory(AccessoryType type) {
        if (activeAccessory == type) {
            activeAccessory = AccessoryType::None;
        } else {
            activeAccessory = type;
        }
    }

    bool isAccessoryActive(AccessoryType type) const {
        return activeAccessory == type;
    }

    bool hasAccessory(AccessoryType type) const {
        for (auto t : accessories) {
            if (t == type) return true;
        }
        return false;
    }

    void addGold(int amount) {
        if (isAccessoryActive(AccessoryType::WealthDiamond)) {
            amount *= 2;
        }
        gold += amount;
    }
    void spendGold(int amount) { gold -= amount; }
    void addExp(int amount) {
        experience += amount;
        if (experience >= level * 100) {
            experience -= level * 100;
            level++;
        }
    }
    bool canUnlockStar(int nextStarLevel) const {
        if (nextStarLevel == 2) return level >= 3;
        if (nextStarLevel == 3) return level >= 5;
        return false;
    }
};