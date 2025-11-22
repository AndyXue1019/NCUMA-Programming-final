#pragma once
#include <map>

#include "Tower.hpp"  // TowerType

struct PlayerStats {
    int gold = 200;       // 初始金幣
    int lives = 20;       // 基地hp
    int level = 1;        // 玩家等級 (用於解鎖塔)
    int experience = 0;   // 經驗值
    int currentWave = 0;  // 當前波數

    std::map<TowerType, int> inventory;

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