#include "GameUI.hpp"

#include <format>  // C++20

#include "Config.hpp"

GameUI::GameUI(const sf::Font& font, PlayerStats& stats)
    : m_font(font), m_playerStats(stats), m_infoText(font) {
    // 背景底色
    m_bg.setSize({static_cast<float>(Config::WINDOW_WIDTH), BAR_HEIGHT});
    m_bg.setPosition({0.f, static_cast<float>(Config::WINDOW_HEIGHT) - BAR_HEIGHT});
    m_bg.setFillColor(sf::Color(30, 30, 30));  // 深灰色
    m_bg.setOutlineColor(sf::Color::White);
    m_bg.setOutlineThickness(2.f);

    // 資訊文字
    m_infoText.setCharacterSize(18);
    m_infoText.setFillColor(sf::Color::Yellow);
    m_infoText.setPosition({20.f, static_cast<float>(Config::WINDOW_HEIGHT) - BAR_HEIGHT + 10.f});

    initButtons();
}

void GameUI::initButtons() {
    float startX = 250.f;  // 按鈕起始 X 座標
    float gap = 10.f;
    float btnSize = 90.f;
    float yPos = static_cast<float>(Config::WINDOW_HEIGHT) - BAR_HEIGHT + 15.f;

    int i = 0;
    for (auto const& [type, info] : TowerData::INFO) {
        Button btn(m_font);
        btn.type = type;

        // 按鈕外框
        btn.shape.setSize({btnSize, btnSize});
        btn.shape.setPosition({startX + i * (btnSize + gap), yPos});
        btn.shape.setFillColor(sf::Color(60, 60, 60));
        btn.shape.setOutlineThickness(2.f);

        // 塔的圖示 (簡單用顏色方塊代表)
        // 實際專案可以在這裡放 Sprite
        btn.shape.setOutlineColor(info.color);

        // 標籤 (塔名稱)
        btn.label.setFont(m_font);
        btn.label.setString(info.name);
        btn.label.setCharacterSize(14);
        btn.label.setFillColor(info.color);
        btn.label.setPosition({btn.shape.getPosition().x + 5.f, btn.shape.getPosition().y + 5.f});

        // 副標籤 (價格/數量)
        btn.subLabel.setFont(m_font);
        btn.subLabel.setCharacterSize(14);
        btn.subLabel.setFillColor(sf::Color::White);
        btn.subLabel.setPosition({btn.shape.getPosition().x + 5.f, btn.shape.getPosition().y + 65.f});

        m_buttons.push_back(std::move(btn));
        i++;
    }
}

void GameUI::updateButtons(bool isShopPhase) {
    for (auto& btn : m_buttons) {
        const auto& info = TowerData::INFO.at(btn.type);

        // 檢查解鎖狀態
        if (m_playerStats.level < info.requiredLevel) {
            btn.locked = true;
            btn.shape.setFillColor(sf::Color(100, 30, 30));  // 紅色鎖定背景
            btn.subLabel.setString(std::format("Lv.{}", info.requiredLevel));
            btn.subLabel.setFillColor(sf::Color::Red);
        } else {
            btn.locked = false;
            btn.shape.setFillColor(sf::Color(60, 60, 60));

            if (isShopPhase) {
                // 商店模式：顯示價格
                btn.subLabel.setString(std::format("${}", info.price));
                // 如果錢不夠，字變紅
                if (m_playerStats.gold < info.price)
                    btn.subLabel.setFillColor(sf::Color::Red);
                else
                    btn.subLabel.setFillColor(sf::Color::Green);
            } else {
                // 戰鬥模式：顯示庫存數量
                int count = m_playerStats.inventory[btn.type];
                btn.subLabel.setString(std::format("x{}", count));
                btn.subLabel.setFillColor(count > 0 ? sf::Color::White : sf::Color(150, 150, 150));
            }
        }
    }
}

void GameUI::draw(sf::RenderWindow& window, bool isShopPhase) {
    // 1. 繪製背景
    window.draw(m_bg);

    // 2. 繪製資訊 (State Info)
    std::string stateStr = isShopPhase ? "[SHOP PHASE]" : "[WAVE INCOMING]";
    std::string info = std::format("{}\nGold: {}\nLives: {}\nLevel: {} Stars",
                                   stateStr, m_playerStats.gold, m_playerStats.lives, m_playerStats.level);
    m_infoText.setString(info);
    window.draw(m_infoText);

    // 3. 更新並繪製按鈕
    updateButtons(isShopPhase);
    for (const auto& btn : m_buttons) {
        window.draw(btn.shape);
        window.draw(btn.label);
        window.draw(btn.subLabel);
    }
}

std::optional<TowerType> GameUI::handleClick(sf::Vector2f mousePos) {
    // 檢查是否點擊到任何按鈕
    for (const auto& btn : m_buttons) {
        if (btn.shape.getGlobalBounds().contains(mousePos)) {
            if (btn.locked) return std::nullopt;  // 鎖定中無法點擊
            return btn.type;
        }
    }
    return std::nullopt;
}