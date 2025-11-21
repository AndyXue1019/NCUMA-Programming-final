#include "WaveManager.hpp"

#include <iostream>

WaveManager::WaveManager(std::vector<std::shared_ptr<Enemy>>& enemies, const std::vector<sf::Vector2f>& path, PlayerStats& stats)
    : m_gameEnemies(enemies), m_path(path), m_playerStats(stats) {
}

void WaveManager::startNextWave() {
    if (m_spawning || m_currentWaveIndex >= 20) return;  // 最多 20 波

    m_currentWaveIndex++;  // 進入下一波 (1-based for display)
    m_playerStats.currentWave = m_currentWaveIndex;

    configureWave(m_currentWaveIndex);

    m_spawning = true;
    m_spawnTimer = 0.f;

    std::cout << "Starting Wave " << m_currentWaveIndex << std::endl;
}

void WaveManager::configureWave(int wave) {
    // 根據 Wave 1-20 設定難度 [cite: 28, 29, 30]

    if (wave == 20) {  // Final Boss
        m_enemiesToSpawn = 1;
        m_spawnInterval = 1.0f;
        m_currentEnemyType = EnemyType::FinalBoss;
    } else if (wave % 5 == 0) {             // Wave 5, 10, 15 (Mini-Boss)
        m_enemiesToSpawn = 1 + (wave / 5);  // Wave 5: 2隻, Wave 10: 3隻...
        m_spawnInterval = 2.0f;
        m_currentEnemyType = EnemyType::MiniBoss;
    } else if (wave > 10) {  // 後期混合 (這裡簡化為 Tank)
        m_enemiesToSpawn = 10 + wave;
        m_spawnInterval = 0.8f;
        m_currentEnemyType = EnemyType::Tank;
    } else if (wave > 5) {  // 中期快速敵人
        m_enemiesToSpawn = 10 + wave;
        m_spawnInterval = 0.5f;
        m_currentEnemyType = EnemyType::Fast;
    } else {  // 初期普通敵人
        m_enemiesToSpawn = 5 + wave;
        m_spawnInterval = 1.0f;
        m_currentEnemyType = EnemyType::Normal;
    }

    m_enemiesAlive = m_enemiesToSpawn;
}

void WaveManager::update(sf::Time dt) {
    if (!m_spawning) return;

    m_spawnTimer += dt.asSeconds();

    if (m_spawnTimer >= m_spawnInterval) {
        if (m_enemiesToSpawn > 0) {
            // 生成敵人
            m_gameEnemies.push_back(std::make_shared<Enemy>(m_path, m_currentEnemyType));
            m_enemiesToSpawn--;
            m_spawnTimer = 0.f;
        } else {
            m_spawning = false;  // 生成完畢，等待所有敵人死亡
        }
    }
}