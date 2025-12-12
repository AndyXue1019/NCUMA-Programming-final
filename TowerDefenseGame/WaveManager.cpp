#include <iostream>

#include "WaveManager.hpp"
#include "Utils.hpp"

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
    m_isRandomWave = false;

    if (wave == 20) {  // Final Boss
        m_enemiesToSpawn = 1;
        m_spawnInterval = 1.0f;
        m_currentEnemyType = EnemyType::FinalBoss;
    } else if (wave % 5 == 0) {             // Wave 5, 10, 15 (Mini-Boss)
        m_enemiesToSpawn = 1 + (wave / 5);  // Wave 5: 2隻, Wave 10: 3隻...
        m_spawnInterval = 2.0f;
        m_currentEnemyType = EnemyType::MiniBoss;
    } else {
        // 隨著波數增加敵人數量
        m_enemiesToSpawn = 5 + wave * 2;

        // 隨著波數減少生成間隔 (越出越快)
        m_spawnInterval = std::max(0.3f, 1.0f - (wave * 0.05f));

        // [關鍵] 設定為隨機波次
        m_isRandomWave = true;
    }

    m_enemiesAlive = m_enemiesToSpawn;
}

void WaveManager::update(sf::Time dt) {
    if (!m_spawning) return;

    m_spawnTimer += dt.asSeconds();

    if (m_spawnTimer >= m_spawnInterval) {
        if (m_enemiesToSpawn > 0) {

            // [新增] 決定要生成的類型
            EnemyType typeToSpawn = m_currentEnemyType; // 預設值 (給 Boss 用)

            if (m_isRandomWave) {
                // 使用 Utils::m_rand() (0-99) 來決定機率
                int r = Utils::m_rand();

                // 這裡設定各種類型的出現機率 (你可以自己調整)
                if (r < 30) {
                    typeToSpawn = EnemyType::Normal;   // 30% 機率是圓形
                }
                else if (r < 55) {
                    typeToSpawn = EnemyType::Fast;     // 25% 機率是快速圓形
                }
                else if (r < 80) {
                    typeToSpawn = EnemyType::Triangle; // 25% 機率是三角形
                }
                else {
                    typeToSpawn = EnemyType::Square;   // 20% 機率是方形
                }
            }

            // 使用決定的類型生成敵人
            m_gameEnemies.push_back(std::make_shared<Enemy>(m_path, typeToSpawn));

            m_enemiesToSpawn--;
            m_spawnTimer = 0.f;
        }
        else {
            m_spawning = false;
        }
    }
}