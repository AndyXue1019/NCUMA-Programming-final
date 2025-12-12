#pragma once
#include <SFML/Graphics.hpp>
#include <cmath> 
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
        m_btnCostLabel(font),
        m_btnSellLabel(font),      // [新增]
        m_btnSellPriceLabel(font)  // [新增]
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
        initSellButton(); // [新增] 初始化賣出按鈕
    }

    void update(sf::Time dt) {
        // 目標位置：選中時為 0，未選中時為 -250
        float targetX = (m_selectedTower != nullptr) ? 0.f : -250.f;
        float speed = 800.f;
        float moveAmount = speed * dt.asSeconds();

        if (m_currentX < targetX) {
            m_currentX += moveAmount;
            if (m_currentX > targetX) m_currentX = targetX;
        }
        else if (m_currentX > targetX) {
            m_currentX -= moveAmount;
            if (m_currentX < targetX) m_currentX = targetX;
        }

        m_bg.setPosition({ m_currentX, 0.f });
        updatePositions(); // 更新文字和按鈕位置跟隨背景
    }

    void draw(sf::RenderWindow& window) {
        if (m_currentX <= -250.f) return; // 完全縮進去就不畫

        window.draw(m_bg);

        if (m_selectedTower) {
            updateInfoText();
            window.draw(m_infoText);

            // 繪製升級按鈕
            window.draw(m_btnShape);
            window.draw(m_btnLabel);
            window.draw(m_btnCostLabel);

            // [新增] 繪製賣出按鈕
            window.draw(m_btnSellShape);
            window.draw(m_btnSellLabel);
            window.draw(m_btnSellPriceLabel);
        }
    }

    // 設定當前選中的塔 (nullptr 代表關閉面板)
    void setSelectedTower(Tower* tower) {
        m_selectedTower = tower;
    }

    // 處理點擊
    bool handleClick(sf::Vector2f mousePos, PlayerStats& stats) {
        if (!m_selectedTower) return false;

        // 1. 檢查升級按鈕
        if (m_btnShape.getGlobalBounds().contains(mousePos)) {
            if (m_selectedTower->isMaxLevel()) return true;

            int cost = m_selectedTower->getUpgradeCost();
            if (stats.gold >= cost) {
                stats.spendGold(cost);
                m_selectedTower->upgrade();
            }
            return true;
        }

        // 2. [新增] 檢查賣出按鈕
        if (m_btnSellShape.getGlobalBounds().contains(mousePos)) {
            // 計算總價值 (公式：基礎價 + 歷次升級總和)
            int level = m_selectedTower->getLevel();
            int price = m_selectedTower->getPrice();
            // 升級到 Level N 的總花費 = Price * (1 + N*(N-1)/2)
            int totalSpent = price * (1 + (level * (level - 1)) / 2);

            // 返還 80%
            int refund = static_cast<int>(totalSpent * 0.8);

            stats.addGold(refund);      // 加錢
            m_selectedTower->destroy(); // 銷毀塔 (Game loop 會自動移除)
            m_selectedTower = nullptr;  // 關閉面板
            return true;
        }

        // 3. 檢查面板背景 (防止穿透)
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

    // [新增] 賣出按鈕元件
    sf::RectangleShape m_btnSellShape;
    sf::Text m_btnSellLabel;
    sf::Text m_btnSellPriceLabel;

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


    void initSellButton() {
        m_btnSellShape.setSize({ 200.f, 60.f });
        m_btnSellShape.setFillColor(sf::Color(180, 50, 50)); // 紅色 (代表危險/刪除)
        m_btnSellShape.setOutlineThickness(1.f);
        m_btnSellShape.setOutlineColor(sf::Color::Black);

        m_btnSellLabel.setFont(m_font);
        m_btnSellLabel.setString("SELL");
        m_btnSellLabel.setCharacterSize(24);
        m_btnSellLabel.setFillColor(sf::Color::White);

        m_btnSellPriceLabel.setFont(m_font);
        m_btnSellPriceLabel.setCharacterSize(18);
        m_btnSellPriceLabel.setFillColor(sf::Color::Yellow); // 顯示返還金額
    }

    void updatePositions() {
        float padding = 25.f;
        m_infoText.setPosition({ m_currentX + padding, 50.f });

        // 升級按鈕
        float btnY = 300.f;
        m_btnShape.setPosition({ m_currentX + padding, btnY });
        m_btnLabel.setPosition({ m_currentX + padding + 40.f, btnY + 5.f });
        m_btnCostLabel.setPosition({ m_currentX + padding + 60.f, btnY + 35.f });

        //售出按鈕
        float sellBtnY = btnY + 60.f + 20.f;
        m_btnSellShape.setPosition({ m_currentX + padding, sellBtnY });
        m_btnSellLabel.setPosition({ m_currentX + padding + 70.f, sellBtnY + 5.f });
        m_btnSellPriceLabel.setPosition({ m_currentX + padding + 60.f, sellBtnY + 35.f });
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

        // 更新升級按鈕狀態
        if (m_selectedTower->isMaxLevel()) {
            m_btnLabel.setString("MAX LEVEL");
            m_btnCostLabel.setString("");
            m_btnShape.setFillColor(sf::Color::Blue); // 滿級變藍色
        }
        else {
            m_btnLabel.setString("UPGRADE");
            m_btnCostLabel.setString(std::format("-${}", m_selectedTower->getUpgradeCost()));
            m_btnShape.setFillColor(sf::Color(0, 150, 0));
        }

        // [新增] 更新賣出按鈕價格
        int level = m_selectedTower->getLevel();
        int price = m_selectedTower->getPrice();
        // 計算累積花費
        int totalSpent = price * (1 + (level * (level - 1)) / 2);
        int refund = static_cast<int>(totalSpent * 0.8);

        m_btnSellPriceLabel.setString(std::format("+${}", refund));
    }
};