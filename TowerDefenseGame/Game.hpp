#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory> // for std::unique_ptr

#include "Config.hpp"
#include "Map.hpp"
#include "Tower.hpp"
#include "Enemy.hpp"
#include "PlayerStats.hpp"
#include "Projectile.hpp"
#include "WaveManager.hpp"

class Game {
   public:
    Game();
    void run();

   private:
    void processEvents();
    void update(sf::Time dt);
    void render();

    // for test use
    void initTestPath();
    std::vector<sf::Vector2f> m_testPath;
    float m_spawnTimer = 0.f;

    // SFML 視窗
    sf::RenderWindow m_window;

    // 地圖實體
    Map m_map;

    // 狀態標記
    bool m_isRunning = true;

    std::vector<std::shared_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Tower>> m_towers;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;

    PlayerStats m_playerStats;
    std::unique_ptr<WaveManager> m_waveManager;

    // UI 資源
    sf::Font m_font;
    sf::Text m_uiText;

    void loadResources();
    void updateUI();

    TowerType m_selectedTowerType;
};