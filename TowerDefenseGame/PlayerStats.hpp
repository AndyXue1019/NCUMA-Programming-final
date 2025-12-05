#pragma once
#include <map>
#include <vector>
#include "Config.hpp"      // 為了讀取 TowerType
#include "Accessory.hpp"   // 飾品定義

struct PlayerStats {
    int gold = 200;
    int lives = 20;
    int level = 1;
    int experience = 0;
    int currentWave = 0;

    std::map<TowerType, int> inventory;

    // 飾品系統
    std::vector<AccessoryType> accessories;
    AccessoryType activeAccessory = AccessoryType::None;

    void equipAccessory(AccessoryType type) {
        if (activeAccessory == type) {
            activeAccessory = AccessoryType::None;
        }
        else {
            activeAccessory = type;
        }
    }

    bool isAccessoryActive(AccessoryType type) const {
        return activeAccessory == type;
    }

    // [補回這個函式] 檢查背包是否擁有某種飾品
    bool hasAccessory(AccessoryType type) const {
        for (auto t : accessories) {
            if (t == type) return true;
        }
        return false;
    }

    void addGold(int amount) { gold += amount; }
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