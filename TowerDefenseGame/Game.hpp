#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "Config.hpp"
#include "Enemy.hpp"
#include "GameUI.hpp"
#include "InventoryPanel.hpp"
#include "Loot.hpp"
#include "Map.hpp"
#include "PlayerStats.hpp"
#include "Projectile.hpp"
#include "Tower.hpp"
#include "UpgradePanel.hpp"
#include "WaveManager.hpp"

// [修正] 加入 Gambler 相關狀態
enum class GameState {
    Shop,
    WaveRunning,
    GameOver,
    GamblerEventPrompt,
    GamblerVideoPlaying
};

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(sf::Time dt);
    void render();

    void initTestPath();
    std::vector<sf::Vector2f> m_testPath;
    float m_spawnTimer = 0.f;

    sf::RenderWindow m_window;
    Map m_map;
    bool m_isRunning = true;

    std::vector<std::shared_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Tower>> m_towers;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;
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

    //宣告順序很重要：Texture 必須在 Sprite 之前
    sf::Texture m_videoTexture;
    sf::Sprite m_videoSprite;

    float m_videoTimer = 0.f;
    float m_videoDuration = 5.0f; // 影片長度

    void loadResources();
    void updateUI();

    void handleShopClick(sf::Vector2f mousePos);
    void handleMapClick(sf::Vector2f mousePos);

    // 加入這些函式宣告
    void checkGamblerEvent();
    void triggerGamblerTransformation();
};