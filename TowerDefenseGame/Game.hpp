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

enum class GameState {
    MainMenu,
    Rules,
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

    void spawnFloatingText(const std::string& str, sf::Vector2f pos, sf::Color color, int size = 20, float lifeTime = 1.0f, sf::Vector2f velocity = { 0.f, -50.f });
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

    sf::Texture m_videoTexture;
    sf::Sprite m_videoSprite;

    sf::Texture m_finishTexture;
    sf::Sprite m_finishSprite;

    sf::Texture m_winTexture;
    sf::Sprite m_winSprite;

    float m_videoTimer = 0.f;
    float m_videoDuration = 5.0f; // 影片長度

    void loadResources();
    void updateUI();

    void handleShopClick(sf::Vector2f mousePos);
    void handleMapClick(sf::Vector2f mousePos);

    void checkGamblerEvent();
    void triggerGamblerTransformation();

    struct FloatingText {
        sf::Text text;
        float lifeTime;
        sf::Vector2f velocity;

        FloatingText(const sf::Font& font, float life, sf::Vector2f vel)
            : text(font), lifeTime(life), velocity(vel) {
        }
    };
    std::vector<FloatingText> m_floatingTexts;

    sf::Text m_titleText;
    sf::Text m_rulesContent;

    MenuButton m_btnStart;
    MenuButton m_btnRules;
    MenuButton m_btnRuleStart;

    // 選單函式
    void initMenu();
    void updateMenu();
};