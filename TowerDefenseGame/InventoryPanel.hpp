#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "PlayerStats.hpp"
#include "Config.hpp"
#include "Accessory.hpp"

class InventoryPanel {
public:
    InventoryPanel(const sf::Font& font, PlayerStats& stats)
        : m_font(font), m_stats(stats), m_tooltipText(font) {

        // 背包按鈕
        m_toggleBtn.setSize({ 50.f, 50.f });
        m_toggleBtn.setOrigin({ 25.f, 25.f });
        m_toggleBtn.setPosition({ static_cast<float>(Config::WINDOW_WIDTH) - 40.f, 40.f });
        m_toggleBtn.setFillColor(sf::Color(100, 100, 200));
        m_toggleBtn.setOutlineThickness(2.f);

        // 背包面板
        m_panelBg.setSize({ 300.f, 400.f });
        m_panelBg.setPosition({ static_cast<float>(Config::WINDOW_WIDTH) - 320.f, 80.f });
        m_panelBg.setFillColor(sf::Color(30, 30, 30, 240));
        m_panelBg.setOutlineThickness(2.f);

        // Tooltip
        m_tooltipBg.setFillColor(sf::Color(10, 10, 10, 200));
        m_tooltipText.setCharacterSize(14);
        m_tooltipText.setFillColor(sf::Color::White);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(m_toggleBtn);

        sf::Text btnText(m_font);
        btnText.setString("BAG");
        btnText.setCharacterSize(14);
        btnText.setOrigin(btnText.getLocalBounds().size / 2.f);
        btnText.setPosition(m_toggleBtn.getPosition());
        window.draw(btnText);

        if (m_isOpen) {
            window.draw(m_panelBg);
            drawGrid(window);

            if (m_showTooltip) {
                window.draw(m_tooltipBg);
                window.draw(m_tooltipText);
            }
        }
    }

    void handleMouseMove(sf::Vector2f mousePos) {
        if (!m_isOpen) return;
        m_showTooltip = false;

        float startX = m_panelBg.getPosition().x + 20.f;
        float startY = m_panelBg.getPosition().y + 20.f;
        float cellSize = 50.f;
        float padding = 10.f;
        int cols = 4;

        for (size_t i = 0; i < m_stats.accessories.size(); ++i) {
            int row = static_cast<int>(i) / cols;
            int col = static_cast<int>(i) % cols;
            float x = startX + col * (cellSize + padding);
            float y = startY + row * (cellSize + padding);

            sf::FloatRect cellBounds({ x, y }, { cellSize, cellSize });
            if (cellBounds.contains(mousePos)) {
                Accessory data = AccessoryData::get(m_stats.accessories[i]);
                showTooltip(data, mousePos);
                return;
            }
        }
    }

    // 修改：新增處理格子點擊的邏輯
    bool handleClick(sf::Vector2f mousePos) {
        // 1. 點擊切換按鈕 (打開/關閉背包)
        if (m_toggleBtn.getGlobalBounds().contains(mousePos)) {
            m_isOpen = !m_isOpen;
            return true;
        }

        if (!m_isOpen) return false;

        // 2. 檢查是否點擊背包背景 (防止穿透)
        if (m_panelBg.getGlobalBounds().contains(mousePos)) {

            // 3. 進一步檢查是否點擊了某個格子 (裝備/卸下)
            float startX = m_panelBg.getPosition().x + 20.f;
            float startY = m_panelBg.getPosition().y + 20.f;
            float cellSize = 50.f;
            float padding = 10.f;
            int cols = 4;

            for (size_t i = 0; i < m_stats.accessories.size(); ++i) {
                int row = static_cast<int>(i) / cols;
                int col = static_cast<int>(i) % cols;
                float x = startX + col * (cellSize + padding);
                float y = startY + row * (cellSize + padding);

                sf::FloatRect cellBounds({ x, y }, { cellSize, cellSize });
                if (cellBounds.contains(mousePos)) {
                    // 點擊飾品 -> 進行裝備切換
                    AccessoryType clickedType = m_stats.accessories[i];
                    m_stats.equipAccessory(clickedType);
                    return true;
                }
            }
            return true; // 點在面板空白處，雖然沒點到飾品，但也算消耗了事件
        }
        return false;
    }

private:
    const sf::Font& m_font;
    PlayerStats& m_stats;

    sf::RectangleShape m_toggleBtn;
    sf::RectangleShape m_panelBg;
    bool m_isOpen = false;

    sf::Text m_tooltipText;
    sf::RectangleShape m_tooltipBg;
    bool m_showTooltip = false;

    void drawGrid(sf::RenderWindow& window) {
        float startX = m_panelBg.getPosition().x + 20.f;
        float startY = m_panelBg.getPosition().y + 20.f;
        float cellSize = 50.f;
        float padding = 10.f;
        int cols = 4;

        for (size_t i = 0; i < m_stats.accessories.size(); ++i) {
            int row = static_cast<int>(i) / cols;
            int col = static_cast<int>(i) % cols;
            float x = startX + col * (cellSize + padding);
            float y = startY + row * (cellSize + padding);
            AccessoryType currentType = m_stats.accessories[i];

            sf::RectangleShape cell({ cellSize, cellSize });
            cell.setPosition({ x, y });
            cell.setFillColor(sf::Color(50, 50, 50));

            // --- 視覺修改：如果這個是「裝備中」的飾品，顯示綠色邊框 ---
            if (m_stats.isAccessoryActive(currentType)) {
                cell.setOutlineColor(sf::Color::Green);
                cell.setOutlineThickness(3.f); // 粗框代表生效
            }
            else {
                cell.setOutlineColor(sf::Color::White);
                cell.setOutlineThickness(1.f);
            }

            window.draw(cell);

            Accessory data = AccessoryData::get(currentType);
            sf::CircleShape icon(15.f);
            icon.setOrigin({ 15.f, 15.f });
            icon.setPosition({ x + cellSize / 2.f, y + cellSize / 2.f });
            icon.setFillColor(data.color);
            window.draw(icon);
        }
    }

    void showTooltip(const Accessory& data, sf::Vector2f mousePos) {
        m_showTooltip = true;

        // 增加一行狀態顯示
        std::string status = (m_stats.activeAccessory == data.type) ? "[EQUIPPED]" : "[Click to Equip]";
        m_tooltipText.setString(data.name + "\n" + status + "\n" + data.description);

        sf::FloatRect bounds = m_tooltipText.getLocalBounds();
        m_tooltipBg.setSize({ bounds.size.x + 20.f, bounds.size.y + 20.f });
        m_tooltipBg.setPosition({ mousePos.x + 15.f, mousePos.y + 15.f });
        m_tooltipText.setPosition({ mousePos.x + 25.f, mousePos.y + 25.f });
    }
};