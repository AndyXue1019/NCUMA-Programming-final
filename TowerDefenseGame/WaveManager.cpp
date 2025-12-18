#include "WaveManager.hpp"

#include <iostream>

#include "Utils.hpp"

WaveManager::WaveManager(std::vector<std::shared_ptr<Enemy>>& enemies, const std::vector<sf::Vector2f>& path, PlayerStats& stats)
    : m_gameEnemies(enemies), m_path(path), m_playerStats(stats) {
}

void WaveManager::startNextWave() {
    if (m_spawning || m_currentWaveIndex >= 20) return;

    m_currentWaveIndex++;
    m_playerStats.currentWave = m_currentWaveIndex;

    configureWave(m_currentWaveIndex);

    m_spawning = true;
    m_spawnTimer = 0.f;

    std::cout << "Starting Wave " << m_currentWaveIndex << std::endl;
}

void WaveManager::configureWave(int wave) {
    m_isRandomWave = false;

    if (wave == 20) {
        m_enemiesToSpawn = 1;
        m_spawnInterval = 1.0f;
        m_currentEnemyType = EnemyType::FinalBoss;
    } else if (wave % 5 == 0) {
        m_enemiesToSpawn = 1 + (wave / 5);
        m_spawnInterval = 2.0f;
        m_currentEnemyType = EnemyType::MiniBoss;
    } else {
        // ÀH¾÷ªi¦¸
        m_enemiesToSpawn = 5 + wave * 2;
        m_spawnInterval = std::max(0.3f, 1.0f - (wave * 0.05f));
        m_isRandomWave = true;
    }

    m_enemiesAlive = m_enemiesToSpawn;
}

void WaveManager::update(sf::Time dt) {
    if (!m_spawning) return;

    m_spawnTimer += dt.asSeconds();

    if (m_spawnTimer >= m_spawnInterval) {
        if (m_enemiesToSpawn > 0) {
            EnemyType typeToSpawn = m_currentEnemyType;

            if (m_isRandomWave) {
                int r = Utils::m_rand();
                if (r < 30) {
                    typeToSpawn = EnemyType::Normal;
                } else if (r < 55) {
                    typeToSpawn = EnemyType::Fast;
                } else if (r < 80) {
                    typeToSpawn = EnemyType::Triangle;
                } else {
                    typeToSpawn = EnemyType::Square;
                }
            }

            m_gameEnemies.push_back(std::make_shared<Enemy>(m_path, typeToSpawn, m_playerStats.hasGambler));
            m_enemiesToSpawn--;
            m_spawnTimer = 0.f;
        } else {
            m_spawning = false;
        }
    }
}