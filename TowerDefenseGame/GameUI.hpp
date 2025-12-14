#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include "PlayerStats.hpp"
#include "TowerData.hpp"

class GameUI {
public:
    GameUI(const sf::Font& font, PlayerStats& stats);
    void draw(sf::RenderWindow& window, bool isShopPhase);
    std::optional<TowerType> handleClick(sf::Vector2f mousePos);

    // 加入這些宣告
    void drawGamblerPrompt(sf::RenderWindow& window);
    int handlePromptClick(sf::Vector2f mousePos);

    static constexpr float BAR_HEIGHT = 120.f;

private:
    const sf::Font& m_font;
    PlayerStats& m_playerStats;

    sf::RectangleShape m_bg;
    sf::Text m_infoText;

    struct Button {
        sf::RectangleShape shape;
        sf::Text label;
        sf::Text subLabel;
        TowerType type;
        bool locked = false;
        Button(const sf::Font& font) : label(font), subLabel(font) {}
    };
    std::vector<Button> m_buttons;

    void initButtons();
    void updateButtons(bool isShopPhase);

    //加入詢問視窗元件宣告
    sf::RectangleShape m_promptBg;
    sf::Text m_promptText;
    sf::RectangleShape m_btnYes;
    sf::Text m_txtYes;
    sf::RectangleShape m_btnNo;
    sf::Text m_txtNo;
    void initPrompt();

	//血量愛心顯示
    sf::Texture m_heartTexture; // 愛心紋理
    sf::Sprite m_heartBg;       // 深色背景愛心 (空血槽)
    sf::Sprite m_heartFill;     // 紅色填充愛心 (當前血量)
    sf::Text m_hpText;          // 顯示百分比文字 (可選)

    // 生成愛心圖片的函式
    void createHeartTexture();
};