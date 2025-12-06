#pragma once
#include <SFML/Graphics.hpp>
#include <format>

#include "Config.hpp"
#include "Tower.hpp"
#include "PlayerStats.hpp"

class UpgradePanel {
public:
    UpgradePanel(const sf::Font& font)
        : m_font(font),
        m_infoText(font),
        m_btnLabel(font),
        m_btnCostLabel(font)
    {
        // 面板背景 (左側 250px 寬)
        m_bg.setSize({ 250.f, static_cast<float>(Config::WINDOW_HEIGHT) });
        m_bg.setFillColor(sf::Color(40, 44, 52, 230)); // 深色半透明
        m_bg.setOutlineThickness(2.f);
        m_bg.setOutlineColor(sf::Color::White);

        // 初始位置在畫面外 (左側)
        m_currentX = -250.f;
        m_bg.setPosition({ m_currentX, 0.f });

        initText();
        initButton();
    }

    void update(sf::Time dt) {
        // 目標位置：選中時為 0，未選中時為 -250
        float targetX = (m_selectedTower != nullptr) ? 0.f : -250.f;
        float speed = 800.f;
        float moveAmount = speed * dt.asSeconds(); // 這一幀預計移動的距離

        // 修正邏輯：移動後檢查是否超過目標，若超過則直接設定為目標值
        if (m_currentX < targetX) {
            m_currentX += moveAmount;
            if (m_currentX > targetX) m_currentX = targetX; // 防止衝過頭
        }
        else if (m_currentX > targetX) {
            m_currentX -= moveAmount;
            if (m_currentX < targetX) m_currentX = targetX; // 防止衝過頭
        }

        m_bg.setPosition({ m_currentX, 0.f });
        updatePositions(); // 更新文字和按鈕位置跟隨背景
    }

    void draw(sf::RenderWindow& window) {
        if (m_currentX <= -250.f) return; // 完全縮進去就不畫

        window.draw(m_bg);

        if (m_selectedTower) {
            updateInfoText(); // 更新數值顯示
            window.draw(m_infoText);

            // 繪製按鈕
            window.draw(m_btnShape);
            window.draw(m_btnLabel);
            window.draw(m_btnCostLabel);
        }
    }

    // 設定當前選中的塔 (nullptr 代表關閉面板)
    void setSelectedTower(Tower* tower) {
        m_selectedTower = tower;
    }

    // 處理點擊升級按鈕
    bool handleClick(sf::Vector2f mousePos, PlayerStats& stats) {
        if (!m_selectedTower) return false;

        // 檢查是否點到按鈕
        if (m_btnShape.getGlobalBounds().contains(mousePos)) {
            if (m_selectedTower->isMaxLevel()) return true;

            int cost = m_selectedTower->getUpgradeCost();
            if (stats.gold >= cost) {
                stats.spendGold(cost);
                m_selectedTower->upgrade();
            }
            return true; // 代表有點擊到 UI，應該攔截事件
        }

        // 檢查是否點到面板背景 (避免點擊穿透到地圖)
        if (m_bg.getGlobalBounds().contains(mousePos)) {
            return true;
        }

        return false;
    }

private:
    const sf::Font& m_font;
    sf::RectangleShape m_bg;
    Tower* m_selectedTower = nullptr;
    float m_currentX;

    sf::Text m_infoText;

    // 升級按鈕
    sf::RectangleShape m_btnShape;
    sf::Text m_btnLabel;
    sf::Text m_btnCostLabel;

    void initText() {
        m_infoText.setFont(m_font);
        m_infoText.setCharacterSize(20);
        m_infoText.setFillColor(sf::Color::White);
    }

    void initButton() {
        m_btnShape.setSize({ 200.f, 60.f });
        m_btnShape.setFillColor(sf::Color(0, 150, 0)); // 綠色

        m_btnLabel.setFont(m_font);
        m_btnLabel.setString("UPGRADE");
        m_btnLabel.setCharacterSize(24);
        m_btnLabel.setFillColor(sf::Color::White);

        m_btnCostLabel.setFont(m_font);
        m_btnCostLabel.setCharacterSize(18);
        m_btnCostLabel.setFillColor(sf::Color::Yellow);
    }

    void updatePositions() {
        float padding = 25.f;
        m_infoText.setPosition({ m_currentX + padding, 50.f });

        float btnY = 300.f;
        m_btnShape.setPosition({ m_currentX + padding, btnY });
        m_btnLabel.setPosition({ m_currentX + padding + 40.f, btnY + 5.f });
        m_btnCostLabel.setPosition({ m_currentX + padding + 60.f, btnY + 35.f });
    }

    void updateInfoText() {
        if (!m_selectedTower) return;

        std::string content = std::format(
            "Type: {}\n\nLevel: {} / 5\n\nDamage: {}\n(+{})",
            m_selectedTower->getName(),
            m_selectedTower->getLevel(),
            m_selectedTower->getDamage(),
            m_selectedTower->isMaxLevel() ? 0 : (m_selectedTower->getNextLevelDamage() - m_selectedTower->getDamage())
        );
        m_infoText.setString(content);

        // 更新按鈕狀態
        if (m_selectedTower->isMaxLevel()) {
            m_btnLabel.setString("MAX LEVEL");
            m_btnCostLabel.setString("");
            m_btnShape.setFillColor(sf::Color::Red);
        }
        else {
            m_btnLabel.setString("UPGRADE");
            m_btnCostLabel.setString(std::format("${}", m_selectedTower->getUpgradeCost()));
            m_btnShape.setFillColor(sf::Color(0, 150, 0));
        }
    }
};