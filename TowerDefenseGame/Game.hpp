#pragma once
#include <SFML/Graphics.hpp>
#include <map> 
#include <memory> 
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
#include "Loot.hpp"
#include "InventoryPanel.hpp"
#include "UpgradePanel.hpp"

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

    sf::RenderWindow m_window;
    Map m_map;
    bool m_isRunning = true;

    std::vector<std::shared_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Tower>> m_towers;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;

    // 掉落物與飾品
    std::vector<std::unique_ptr<Loot>> m_loots;

    PlayerStats m_playerStats;
    std::unique_ptr<WaveManager> m_waveManager;

    GameState m_gameState = GameState::Shop;
    std::optional<TowerType> m_selectedTower;

    std::unique_ptr<GameUI> m_gameUI;
    std::unique_ptr<UpgradePanel> m_upgradePanel;
    std::unique_ptr<InventoryPanel> m_inventoryPanel;

    sf::Font m_font;
    sf::Text m_uiText;

    // 移除 m_towerTextures

    void loadResources();
    void updateUI();

    TowerType m_selectedTowerType;

    void handleShopClick(sf::Vector2f mousePos);
    void handleMapClick(sf::Vector2f mousePos);
};