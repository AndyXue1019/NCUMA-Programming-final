#include "GameUI.hpp"

#include <format>
#include <iostream>

#include "Config.hpp"
#include "TowerData.hpp"

// 定義金色
static const sf::Color GOLD_COLOR(255, 215, 0);

GameUI::GameUI(const sf::Font& font, PlayerStats& stats)
    : m_font(font),
      m_playerStats(stats),
      m_infoText(font),
      m_promptText(font),
      m_txtYes(font),
      m_txtNo(font),
      m_hpText(font),
      m_heartBg(m_heartTexture),
      m_heartFill(m_heartTexture) {
    m_bg.setSize({static_cast<float>(Config::WINDOW_WIDTH), BAR_HEIGHT});
    m_bg.setPosition({0.f, static_cast<float>(Config::WINDOW_HEIGHT) - BAR_HEIGHT});
    m_bg.setFillColor(sf::Color(30, 30, 30));
    m_bg.setOutlineColor(sf::Color::White);
    m_bg.setOutlineThickness(2.f);

    m_infoText.setCharacterSize(18);
    m_infoText.setFillColor(sf::Color::Yellow);
    m_infoText.setPosition({20.f, static_cast<float>(Config::WINDOW_HEIGHT) - BAR_HEIGHT + 10.f});

    // 產生愛心並設定位置
    createHeartTexture();

    // 設定愛心位置 (右下角)
    float heartX = static_cast<float>(Config::WINDOW_WIDTH) - 130.f;
    float heartY = static_cast<float>(Config::WINDOW_HEIGHT) - 110.f;

    // Sprite 已經綁定 Texture，這裡只要設定位置顏色
    m_heartBg.setPosition({heartX, heartY});
    m_heartBg.setColor(sf::Color(50, 0, 0, 150));  // 深紅色半透明 (背景)

    m_heartFill.setPosition({heartX, heartY});
    m_heartFill.setColor(sf::Color(255, 0, 0));  // 亮紅色 (血量)

    // 血量文字 (顯示在愛心中央)
    m_hpText.setCharacterSize(20);
    m_hpText.setFillColor(sf::Color::White);
    m_hpText.setOutlineColor(sf::Color::Black);
    m_hpText.setOutlineThickness(1.f);

    initButtons();
    initPrompt();
}

void GameUI::initButtons() {
    m_buttons.clear();
    float startX = 250.f;
    float gap = 10.f;
    float btnSize = 90.f;
    float yPos = static_cast<float>(Config::WINDOW_HEIGHT) - BAR_HEIGHT + 15.f;

    int i = 0;
    for (auto const& [type, info] : TowerData::INFO) {
        if (type == TowerType::Gambler) continue;

        Button btn(m_font);
        btn.type = type;
        btn.shape.setSize({btnSize, btnSize});
        btn.shape.setPosition({startX + i * (btnSize + gap), yPos});
        btn.shape.setFillColor(sf::Color(60, 60, 60));
        btn.shape.setOutlineThickness(2.f);
        btn.shape.setOutlineColor(info.color);

        btn.label.setString(info.name);
        btn.label.setCharacterSize(14);
        btn.label.setFillColor(info.color);
        btn.label.setPosition({btn.shape.getPosition().x + 5.f, btn.shape.getPosition().y + 5.f});

        btn.subLabel.setCharacterSize(14);
        btn.subLabel.setFillColor(sf::Color::White);
        btn.subLabel.setPosition({btn.shape.getPosition().x + 5.f, btn.shape.getPosition().y + 65.f});

        m_buttons.push_back(std::move(btn));
        i++;
    }
}

void GameUI::initPrompt() {
    float w = 600.f;
    float h = 300.f;
    m_promptBg.setSize({w, h});
    m_promptBg.setOrigin({w / 2, h / 2});
    m_promptBg.setPosition({Config::WINDOW_WIDTH / 2.f, Config::WINDOW_HEIGHT / 2.f});
    m_promptBg.setFillColor(sf::Color(20, 20, 20, 240));
    m_promptBg.setOutlineThickness(3.f);
    m_promptBg.setOutlineColor(GOLD_COLOR);

    m_promptText.setString("Spend ALL GOLD to buy\n'God of Gamblers'?\n(Requires 100,000 Gold)");
    m_promptText.setCharacterSize(30);
    m_promptText.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = m_promptText.getLocalBounds();
    m_promptText.setOrigin({textBounds.size.x / 2, textBounds.size.y / 2});
    m_promptText.setPosition({m_promptBg.getPosition().x, m_promptBg.getPosition().y - 50.f});

    // Yes Button
    m_btnYes.setSize({150.f, 60.f});
    m_btnYes.setOrigin({75.f, 30.f});
    m_btnYes.setPosition({m_promptBg.getPosition().x - 100.f, m_promptBg.getPosition().y + 80.f});
    m_btnYes.setFillColor(sf::Color(0, 150, 0));

    m_txtYes.setString("YES");
    m_txtYes.setCharacterSize(24);
    sf::FloatRect yb = m_txtYes.getLocalBounds();
    m_txtYes.setOrigin({yb.size.x / 2, yb.size.y / 2});
    m_txtYes.setPosition(m_btnYes.getPosition());

    // No Button
    m_btnNo.setSize({150.f, 60.f});
    m_btnNo.setOrigin({75.f, 30.f});
    m_btnNo.setPosition({m_promptBg.getPosition().x + 100.f, m_promptBg.getPosition().y + 80.f});
    m_btnNo.setFillColor(sf::Color(150, 0, 0));

    m_txtNo.setString("NO");
    m_txtNo.setCharacterSize(24);
    sf::FloatRect nb = m_txtNo.getLocalBounds();
    m_txtNo.setOrigin({nb.size.x / 2, nb.size.y / 2});
    m_txtNo.setPosition(m_btnNo.getPosition());
}

void GameUI::updateButtons(bool isShopPhase) {
    if (m_playerStats.hasGambler) {
        bool hasGamblerBtn = false;
        for (auto& btn : m_buttons)
            if (btn.type == TowerType::Gambler) hasGamblerBtn = true;

        if (!hasGamblerBtn) {
            Button btn(m_font);
            btn.type = TowerType::Gambler;
            float btnSize = 90.f;
            float gap = 10.f;
            float startX = 250.f + 6 * (btnSize + gap);
            float yPos = static_cast<float>(Config::WINDOW_HEIGHT) - BAR_HEIGHT + 15.f;

            btn.shape.setSize({btnSize, btnSize});
            btn.shape.setPosition({startX, yPos});
            btn.shape.setFillColor(sf::Color::Black);
            btn.shape.setOutlineThickness(2.f);
            btn.shape.setOutlineColor(GOLD_COLOR);

            btn.label.setString("GOD");
            btn.label.setPosition({btn.shape.getPosition().x + 5.f, btn.shape.getPosition().y + 5.f});
            btn.label.setFillColor(GOLD_COLOR);

            btn.subLabel.setString("x1");
            btn.subLabel.setPosition({btn.shape.getPosition().x + 5.f, btn.shape.getPosition().y + 65.f});

            m_buttons.push_back(std::move(btn));
        }
    }

    for (auto& btn : m_buttons) {
        const auto& info = TowerData::INFO.at(btn.type);

        if (m_playerStats.hasGambler && btn.type != TowerType::Gambler) {
            btn.locked = true;
            btn.shape.setFillColor(sf::Color::Black);
            btn.subLabel.setString("LOCKED");
            btn.subLabel.setFillColor(sf::Color(100, 100, 100));
            continue;
        }

        if (btn.type == TowerType::Gambler) {
            btn.locked = false;
            int count = m_playerStats.inventory[TowerType::Gambler];
            if (count > 0) {
                btn.subLabel.setString("OWNED");
                btn.subLabel.setFillColor(sf::Color::Green);
            } else {
                btn.locked = true;
                btn.shape.setFillColor(sf::Color::Black);
                btn.subLabel.setString("LOCKED");
                btn.subLabel.setFillColor(sf::Color(100, 100, 100));
                btn.subLabel.setCharacterSize(14);
            }
            continue;
        }

        if (m_playerStats.level < info.requiredLevel) {
            btn.locked = true;
            btn.shape.setFillColor(sf::Color(100, 30, 30));
            btn.subLabel.setString(std::format("Lv.{}", info.requiredLevel));
            btn.subLabel.setFillColor(sf::Color::Red);
        } else {
            btn.locked = false;
            btn.shape.setFillColor(sf::Color(60, 60, 60));

            if (isShopPhase) {
                btn.subLabel.setString(std::format("${}", info.price));
                if (m_playerStats.gold < info.price)
                    btn.subLabel.setFillColor(sf::Color::Red);
                else
                    btn.subLabel.setFillColor(sf::Color::Green);
            } else {
                int count = m_playerStats.inventory[btn.type];
                btn.subLabel.setString(std::format("x{}", count));
                btn.subLabel.setFillColor(count > 0 ? sf::Color::White : sf::Color(150, 150, 150));
            }
        }
    }
}

void GameUI::createHeartTexture() {
    sf::Image img;
    // SFML 3.0 使用 resize
    img.resize({100, 100}, sf::Color::Transparent);

    float cx = 50.f;
    float cy = 55.f;
    float scale = 45.f;

    for (unsigned int y = 0; y < 100; ++y) {
        for (unsigned int x = 0; x < 100; ++x) {
            float nx = (x - cx) / scale;
            float ny = (cy - y) / scale;

            float a = nx * nx + ny * ny - 1.f;
            if (a * a * a - nx * nx * ny * ny * ny <= 0.f) {
                img.setPixel({x, y}, sf::Color::White);
            }
        }
    }
    bool loaded = m_heartTexture.loadFromImage(img);
    if (!loaded) {
        std::cerr << "Failed to create heart texture!" << std::endl;
    }
    m_heartTexture.setSmooth(true);
}

void GameUI::draw(sf::RenderWindow& window, bool isShopPhase) {
    window.draw(m_bg);
    std::string stateStr = isShopPhase ? "[SHOP PHASE]" : "[WAVE INCOMING]";
    std::string info = std::format("{}\nGold: {}\nLevel: {} Stars",
                                   stateStr, m_playerStats.gold, m_playerStats.level);
    m_infoText.setString(info);
    window.draw(m_infoText);

    updateButtons(isShopPhase);
    for (const auto& btn : m_buttons) {
        window.draw(btn.shape);
        window.draw(btn.label);
        window.draw(btn.subLabel);
    }

    // 繪製愛心
    float maxLives = 20.f;
    float pct = static_cast<float>(m_playerStats.lives) / maxLives;
    if (pct < 0.f) pct = 0.f;
    if (pct > 1.f) pct = 1.f;

    window.draw(m_heartBg);

    unsigned int texH = m_heartTexture.getSize().y;
    unsigned int texW = m_heartTexture.getSize().x;

    int fillHeight = static_cast<int>(texH * pct);
    int topOffset = texH - fillHeight;

    m_heartFill.setTextureRect(sf::IntRect({0, topOffset}, {static_cast<int>(texW), fillHeight}));

    sf::Vector2f basePos = m_heartBg.getPosition();
    m_heartFill.setPosition({basePos.x, basePos.y + topOffset});

    window.draw(m_heartFill);

    m_hpText.setString(std::format("{}%", static_cast<int>(pct * 100)));
    sf::FloatRect textBounds = m_hpText.getLocalBounds();
    m_hpText.setOrigin({textBounds.size.x / 2.f, textBounds.size.y / 2.f});
    m_hpText.setPosition({basePos.x + 50.f, basePos.y + 50.f});
    window.draw(m_hpText);
}

void GameUI::drawGamblerPrompt(sf::RenderWindow& window) {
    window.draw(m_promptBg);
    window.draw(m_promptText);
    window.draw(m_btnYes);
    window.draw(m_txtYes);
    window.draw(m_btnNo);
    window.draw(m_txtNo);
}

int GameUI::handlePromptClick(sf::Vector2f mousePos) {
    if (m_btnYes.getGlobalBounds().contains(mousePos)) return 1;
    if (m_btnNo.getGlobalBounds().contains(mousePos)) return 0;
    return -1;
}

std::optional<TowerType> GameUI::handleClick(sf::Vector2f mousePos) {
    for (const auto& btn : m_buttons) {
        if (btn.shape.getGlobalBounds().contains(mousePos)) {
            if (btn.locked) return std::nullopt;
            return btn.type;
        }
    }
    return std::nullopt;
}

MenuButton::MenuButton(const sf::Font& font) : text(font) {}

void MenuButton::init(const std::string& str, sf::Vector2f pos, sf::Vector2f size) {
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
    text.setOrigin({bounds.size.x / 2.f + bounds.position.x, bounds.size.y / 2.f + bounds.position.y});
    text.setPosition(pos);
}

void MenuButton::update(sf::Vector2f mousePos) {
    if (shape.getGlobalBounds().contains(mousePos)) {
        shape.setScale({1.1f, 1.1f});
        text.setScale({1.1f, 1.1f});
        shape.setOutlineColor(sf::Color::Yellow);
        text.setFillColor(sf::Color::Yellow);
    } else {
        shape.setScale({1.0f, 1.0f});
        text.setScale({1.0f, 1.0f});
        shape.setOutlineColor(sf::Color::White);
        text.setFillColor(sf::Color::White);
    }
}

void MenuButton::draw(sf::RenderWindow& window) {
    window.draw(shape);
    window.draw(text);
}

bool MenuButton::isClicked(sf::Vector2f mousePos) const {
    return shape.getGlobalBounds().contains(mousePos);
}