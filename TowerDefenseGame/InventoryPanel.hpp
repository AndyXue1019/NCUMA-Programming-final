#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <format>
#include <cstdint> // 新增：為了使用 std::uint8_t
#include "PlayerStats.hpp"
#include "Config.hpp"
#include "Accessory.hpp"

class InventoryPanel {
public:
    InventoryPanel(const sf::Font& font, PlayerStats& stats)
        : m_font(font), m_stats(stats), m_tooltipText(font), m_trashText(font) {

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

        // 垃圾桶區域
        float panelX = m_panelBg.getPosition().x;
        float panelY = m_panelBg.getPosition().y;
        float panelW = m_panelBg.getSize().x;
        float panelH = m_panelBg.getSize().y;

        m_trashSlot.setSize({ 60.f, 60.f });
        m_trashSlot.setOrigin({ 30.f, 30.f });
        m_trashSlot.setPosition({ panelX + panelW - 50.f, panelY + panelH - 50.f });
        m_trashSlot.setFillColor(sf::Color(200, 50, 50));
        m_trashSlot.setOutlineColor(sf::Color::White);
        m_trashSlot.setOutlineThickness(2.f);

        m_trashText.setString("SELL");
        m_trashText.setCharacterSize(14);
        m_trashText.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = m_trashText.getLocalBounds();
        m_trashText.setOrigin({ textBounds.size.x / 2.f, textBounds.size.y / 2.f });
        m_trashText.setPosition(m_trashSlot.getPosition());

        // Tooltip
        m_tooltipBg.setFillColor(sf::Color(10, 10, 10, 200));
        m_tooltipText.setCharacterSize(14);
        m_tooltipText.setFillColor(sf::Color::White);

        m_draggedIcon.setRadius(15.f);
        m_draggedIcon.setOrigin({ 15.f, 15.f });
    }

    void update(sf::Time dt) {
        for (auto it = m_floatingTexts.begin(); it != m_floatingTexts.end(); ) {
            it->lifeTime -= dt.asSeconds();
            it->text.move({ 0.f, -50.f * dt.asSeconds() });

            sf::Color c = it->text.getFillColor();
            // 修正：使用 std::uint8_t 取代 sf::Uint8
            c.a = static_cast<std::uint8_t>(255 * (it->lifeTime / 1.0f));
            it->text.setFillColor(c);

            if (it->lifeTime <= 0.f) {
                it = m_floatingTexts.erase(it);
            }
            else {
                ++it;
            }
        }
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

            window.draw(m_trashSlot);
            window.draw(m_trashText);

            for (const auto& ft : m_floatingTexts) {
                window.draw(ft.text);
            }

            if (m_isDragging) {
                window.draw(m_draggedIcon);
            }
            else if (m_showTooltip) {
                window.draw(m_tooltipBg);
                window.draw(m_tooltipText);
            }
        }
    }

    void close() {
        m_isOpen = false;
        m_isDragging = false; // 關閉時也要取消拖曳狀態，避免 Bug
        m_draggedIndex = -1;
    }

    void handleMouseMove(sf::Vector2f mousePos) {
        if (!m_isOpen) return;

        if (m_isDragging) {
            m_draggedIcon.setPosition(mousePos);
            m_showTooltip = false;
            return;
        }

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

    bool handleMousePress(sf::Vector2f mousePos) {
        if (m_toggleBtn.getGlobalBounds().contains(mousePos)) {
            m_isOpen = !m_isOpen;
            if (!m_isOpen) m_isDragging = false;
            return true;
        }

        if (!m_isOpen) return false;

        if (m_panelBg.getGlobalBounds().contains(mousePos)) {
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
                    m_isDragging = true;
                    m_draggedIndex = static_cast<int>(i);
                    Accessory data = AccessoryData::get(m_stats.accessories[i]);
                    m_draggedIcon.setFillColor(data.color);
                    m_draggedIcon.setPosition(mousePos);
                    return true;
                }
            }
            return true;
        }
        return false;
    }

    bool handleMouseRelease(sf::Vector2f mousePos) {
        if (!m_isOpen) return false;

        if (m_isDragging) {
            m_isDragging = false;

            if (m_trashSlot.getGlobalBounds().contains(mousePos)) {
                sellItem(m_draggedIndex);
                return true;
            }

            if (m_draggedIndex >= 0 && m_draggedIndex < m_stats.accessories.size()) {
                m_stats.equipAccessory(m_stats.accessories[m_draggedIndex]);
            }

            m_draggedIndex = -1;
            return true;
        }
        return false;
    }

private:
    const sf::Font& m_font;
    PlayerStats& m_stats;

    sf::RectangleShape m_toggleBtn;
    sf::RectangleShape m_panelBg;
    bool m_isOpen = false;

    sf::RectangleShape m_trashSlot;
    sf::Text m_trashText;

    bool m_isDragging = false;
    int m_draggedIndex = -1;
    sf::CircleShape m_draggedIcon;

    struct FloatingText {
        sf::Text text;
        float lifeTime;

        // 修正：新增建構函式，確保 sf::Text 有被正確初始化
        FloatingText(const sf::Font& font) : text(font), lifeTime(1.0f) {}
    };
    std::vector<FloatingText> m_floatingTexts;

    sf::Text m_tooltipText;
    sf::RectangleShape m_tooltipBg;
    bool m_showTooltip = false;

    void sellItem(int index) {
        if (index < 0 || index >= m_stats.accessories.size()) return;

        AccessoryType type = m_stats.accessories[index];
        Accessory data = AccessoryData::get(type);

        // 1. 增加金錢與特效
        m_stats.addGold(data.price);
        spawnFloatingText(std::format("+{}", data.price), m_trashSlot.getPosition());

        // 2. [關鍵修改] 先從背包移除該寶石
        m_stats.accessories.erase(m_stats.accessories.begin() + index);

        // 3. 檢查裝備狀態
        // 如果賣掉的類型剛好是目前裝備的類型
        if (m_stats.activeAccessory == type) {
            // 檢查背包裡是否「還有」同類型的寶石
            // m_stats.hasAccessory 是我們之前在 PlayerStats 寫好的 Helper
            if (!m_stats.hasAccessory(type)) {
                // 如果已經沒有庫存了，才強制卸下
                m_stats.activeAccessory = AccessoryType::None;
            }
            // 如果還有庫存 (hasAccessory 回傳 true)，則不做任何事，保持裝備狀態
        }

        // 重置拖曳索引
        m_draggedIndex = -1;
    }

    void spawnFloatingText(const std::string& str, sf::Vector2f pos) {
        // 修正：在建立物件時直接傳入 font
        FloatingText ft(m_font);

        // ft.text.setFont(m_font); // 這行不再需要，因為建構子已經做了
        ft.text.setString(str);
        ft.text.setCharacterSize(24);
        ft.text.setFillColor(sf::Color::Yellow);
        ft.text.setOutlineColor(sf::Color::Black);
        ft.text.setOutlineThickness(1.f);

        sf::FloatRect bounds = ft.text.getLocalBounds();
        ft.text.setOrigin({ bounds.size.x / 2.f, bounds.size.y / 2.f });
        ft.text.setPosition({ pos.x, pos.y - 20.f });

        m_floatingTexts.push_back(std::move(ft));
    }

    void drawGrid(sf::RenderWindow& window) {
        float startX = m_panelBg.getPosition().x + 20.f;
        float startY = m_panelBg.getPosition().y + 20.f;
        float cellSize = 50.f;
        float padding = 10.f;
        int cols = 4;

        for (size_t i = 0; i < m_stats.accessories.size(); ++i) {
            bool isBeingDragged = (m_isDragging && m_draggedIndex == static_cast<int>(i));

            int row = static_cast<int>(i) / cols;
            int col = static_cast<int>(i) % cols;
            float x = startX + col * (cellSize + padding);
            float y = startY + row * (cellSize + padding);
            AccessoryType currentType = m_stats.accessories[i];

            sf::RectangleShape cell({ cellSize, cellSize });
            cell.setPosition({ x, y });
            cell.setFillColor(sf::Color(50, 50, 50));

            if (m_stats.isAccessoryActive(currentType)) {
                cell.setOutlineColor(sf::Color::Green);
                cell.setOutlineThickness(3.f);
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

            sf::Color iconColor = data.color;
            if (isBeingDragged) {
                iconColor.a = 100;
            }
            icon.setFillColor(iconColor);
            window.draw(icon);
        }
    }

    void showTooltip(const Accessory& data, sf::Vector2f mousePos) {
        m_showTooltip = true;
        std::string status = (m_stats.activeAccessory == data.type) ? "[EQUIPPED]" : "[Drag to Sell / Click to Equip]";
        m_tooltipText.setString(std::format("{}\nPrice: ${}\n{}\n{}", data.name, data.price, status, data.description));

        sf::FloatRect bounds = m_tooltipText.getLocalBounds();
        m_tooltipBg.setSize({ bounds.size.x + 20.f, bounds.size.y + 20.f });
        m_tooltipBg.setPosition({ mousePos.x + 15.f, mousePos.y + 15.f });
        m_tooltipText.setPosition({ mousePos.x + 25.f, mousePos.y + 25.f });
    }
};