#pragma once
#include <SFML/System/Time.hpp>
#include <memory>
#include <vector>

#include "Enemy.hpp"
#include "PlayerStats.hpp"

class WaveManager {
   public:
    // 需要傳入敵人容器，以便生成敵人
    WaveManager(std::vector<std::shared_ptr<Enemy>>& enemies, const std::vector<sf::Vector2f>& path, PlayerStats& stats);

    void update(sf::Time dt);
    void startNextWave();

    bool isWaveInProgress() const { return m_spawning || m_enemiesAlive > 0; }
    void notifyEnemyDeath() {
        if (m_enemiesAlive > 0) m_enemiesAlive--;
    }
    void notifyEnemyReachedEnd() {
        if (m_enemiesAlive > 0) m_enemiesAlive--;
    }

   private:
    std::vector<std::shared_ptr<Enemy>>& m_gameEnemies;
    const std::vector<sf::Vector2f>& m_path;
    PlayerStats& m_playerStats;

    // 波次設定
    struct WaveConfig {
        int count;
        float interval;
        EnemyType type;
    };

    int m_currentWaveIndex = 0;  // 0-19 (代表 Wave 1-20)
    bool m_spawning = false;

    // 生成計時
    float m_spawnTimer = 0.f;
    float m_spawnInterval = 1.0f;
    int m_enemiesToSpawn = 0;
    EnemyType m_currentEnemyType = EnemyType::Normal;

    // 追蹤場上敵人數量，判斷波次是否結束
    int m_enemiesAlive = 0;

    // 設定該波次的內容
    void configureWave(int waveIndex);
};