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

//加入 Gambler 相關狀態
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

    void spawnFloatingText(const std::string& str, sf::Vector2f pos, sf::Color color, int size = 20);
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

        FloatingText(const sf::Font& font)
            : text(font), lifeTime(1.0f), velocity({ 0.f, -50.f }) {
        }
    };
    std::vector<FloatingText> m_floatingTexts;

    //我把這個全部丟在這邊應該不會被打吧
    struct MenuButton {
        sf::RectangleShape shape;
        sf::Text text;

        MenuButton(const sf::Font& font) : text(font) {}

        void init(const std::string& str, sf::Vector2f pos, sf::Vector2f size) {
            shape.setSize(size);
            shape.setOrigin(size / 2.f);
            shape.setPosition(pos);
            shape.setFillColor(sf::Color(50, 50, 50));
            shape.setOutlineColor(sf::Color::White);
            shape.setOutlineThickness(2.f);


            text.setString(sf::String::fromUtf8(str.begin(), str.end()));
            text.setCharacterSize(24);
            text.setFillColor(sf::Color::White);

            sf::FloatRect bounds = text.getLocalBounds();
            text.setOrigin({ bounds.size.x / 2.f + bounds.position.x, bounds.size.y / 2.f + bounds.position.y });
            text.setPosition(pos);
        }

        // 更新互動 (滑鼠懸停放大)
        void update(sf::Vector2f mousePos) {
            if (shape.getGlobalBounds().contains(mousePos)) {
                shape.setScale({ 1.1f, 1.1f });
                text.setScale({ 1.1f, 1.1f });
                shape.setOutlineColor(sf::Color::Yellow);
                text.setFillColor(sf::Color::Yellow);
            }
            else {
                shape.setScale({ 1.0f, 1.0f });
                text.setScale({ 1.0f, 1.0f });
                shape.setOutlineColor(sf::Color::White);
                text.setFillColor(sf::Color::White);
            }
        }

        void draw(sf::RenderWindow& window) {
            window.draw(shape);
            window.draw(text);
        }

        bool isClicked(sf::Vector2f mousePos) const {
            return shape.getGlobalBounds().contains(mousePos);
        }
    };

    sf::Text m_titleText;
    sf::Text m_rulesContent;

    MenuButton m_btnStart;
    MenuButton m_btnRules;
    MenuButton m_btnRuleStart;

    // 選單函式
    void initMenu();
    void updateMenu();
};