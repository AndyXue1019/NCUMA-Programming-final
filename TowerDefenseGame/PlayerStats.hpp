#pragma once

struct PlayerStats {
    int gold = 200;       // 初始金幣
    int lives = 20;       // 基地hp
    int level = 1;        // 玩家等級 (用於解鎖塔)
    int experience = 0;   // 經驗值
    int currentWave = 0;  // 當前波數

    void addGold(int amount) { gold += amount; }
    void spendGold(int amount) { gold -= amount; }
    void addExp(int amount) {
        experience += amount;
        // 簡單的升級邏輯：每 100 經驗升一級
        if (experience >= level * 100) {
            experience -= level * 100;
            level++;
        }
    }
};