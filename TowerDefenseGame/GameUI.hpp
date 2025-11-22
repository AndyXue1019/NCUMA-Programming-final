#pragma once
#include <SFML/Graphics.hpp>
#include <optional>

#include "PlayerStats.hpp"
#include "TowerData.hpp"

class GameUI {
   public:
    // 需要載入字型
    GameUI(const sf::Font& font, PlayerStats& stats);

    // 繪製 UI (根據是否在商店階段，顯示不同內容)
    void draw(sf::RenderWindow& window, bool isShopPhase);

    // 處理點擊：回傳被點擊的 TowerType (如果沒點到則回傳 nullopt)
    std::optional<TowerType> handleClick(sf::Vector2f mousePos);

    // 設定 UI 區域的高度
    static constexpr float BAR_HEIGHT = 120.f;

   private:
    const sf::Font& m_font;
    PlayerStats& m_playerStats;

    sf::RectangleShape m_bg;
    sf::Text m_infoText;  // 顯示金錢、等級等

    // 按鈕區域定義
    struct Button {
        sf::RectangleShape shape;
        sf::Text label;
        sf::Text subLabel;  // 顯示價格或數量
        TowerType type;
        bool locked = false;
        Button(const sf::Font& font) : label(font), subLabel(font) {}
    };


    std::vector<Button> m_buttons;

    void initButtons();
    void updateButtons(bool isShopPhase);
};