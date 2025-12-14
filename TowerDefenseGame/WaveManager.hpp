#pragma once
#include <SFML/System/Time.hpp>
#include <memory>
#include <vector>
#include "Enemy.hpp"
#include "PlayerStats.hpp"

class WaveManager {
public:
    WaveManager(std::vector<std::shared_ptr<Enemy>>& enemies, const std::vector<sf::Vector2f>& path, PlayerStats& stats);

    void update(sf::Time dt);
    void startNextWave();

    bool isWaveInProgress() const { return m_spawning || m_enemiesAlive > 0; }
    void notifyEnemyDeath() { if (m_enemiesAlive > 0) m_enemiesAlive--; }
    void notifyEnemyReachedEnd() { if (m_enemiesAlive > 0) m_enemiesAlive--; }

    //加入此函式
    void forceEndWave() {
        m_spawning = false;
        m_enemiesAlive = 0;
        m_spawnTimer = 0.f;
    }

private:
    std::vector<std::shared_ptr<Enemy>>& m_gameEnemies;
    const std::vector<sf::Vector2f>& m_path;
    PlayerStats& m_playerStats;

    struct WaveConfig {
        int count;
        float interval;
        EnemyType type;
    };

    int m_currentWaveIndex = 0;
    bool m_spawning = false;
    float m_spawnTimer = 0.f;
    float m_spawnInterval = 1.0f;
    int m_enemiesToSpawn = 0;
    EnemyType m_currentEnemyType = EnemyType::Normal;
    int m_enemiesAlive = 0;

    bool m_isRandomWave = false;

    void configureWave(int waveIndex);
};