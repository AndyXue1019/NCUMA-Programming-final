#pragma once
#include <map>

#include "Tower.hpp"  // TowerType
#include "Config.hpp"
#include "Accessory.hpp"

struct PlayerStats {
    int gold = 200;       // 初始金幣
    int lives = 20;       // 基地hp
    int level = 1;        // 玩家等級 (用於解鎖塔)
    int experience = 0;   // 經驗值
    int currentWave = 0;  // 當前波數

    std::map<TowerType, int> inventory;
    std::vector<AccessoryType> accessories;
    AccessoryType activeAccessory = AccessoryType::None;

    // 新增：裝備/切換飾品的方法
    void equipAccessory(AccessoryType type) {
        // 如果點擊的是已經裝備的，可以選擇「卸下」(變成 None) 或是「保持裝備」
        // 這裡我們先做：如果點擊已裝備的就卸下；點擊其他的就換裝
        if (activeAccessory == type) {
            activeAccessory = AccessoryType::None; // 卸下
        }
        else {
            activeAccessory = type; // 換裝 (會自動覆蓋舊的)
        }
    }

    // 用來給塔判斷是否該發動效果
    bool isAccessoryActive(AccessoryType type) const {
        return activeAccessory == type;
    }

    void addGold(int amount) { gold += amount; }
    void spendGold(int amount) { gold -= amount; }

    void addExp(int amount) {
        experience += amount;
        // 每 100 經驗升一級
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