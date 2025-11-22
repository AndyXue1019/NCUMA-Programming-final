#pragma once
#include <SFML/Graphics.hpp>
#include <memory> // for std::unique_ptr and std::shared_ptr
#include <optional>
#include <vector>

#include "Config.hpp"
#include "Enemy.hpp"
#include "GameUI.hpp"
#include "Map.hpp"
#include "PlayerStats.hpp"
#include "Projectile.hpp"
#include "Tower.hpp"
#include "WaveManager.hpp"

enum class GameState { Shop, WaveRunning, GameOver };

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

    GameState m_gameState = GameState::Shop;
    std::optional<TowerType> m_selectedTower;

    
    // UI 資源
    std::unique_ptr<GameUI> m_gameUI;

    sf::Font m_font;
    sf::Text m_uiText;

    void loadResources();
    void updateUI();

    TowerType m_selectedTowerType;

    void handleShopClick(sf::Vector2f mousePos);
    void handleMapClick(sf::Vector2f mousePos);
};