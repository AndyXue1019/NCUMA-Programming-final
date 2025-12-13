#include "GameUI.hpp"
#include <format>
#include "Config.hpp"
#include "TowerData.hpp"

// 定義金色
static const sf::Color GOLD_COLOR(255, 215, 0);

GameUI::GameUI(const sf::Font& font, PlayerStats& stats)
    : m_font(font), m_playerStats(stats), m_infoText(font),
    // [關鍵修正] 所有 Text 都必須在這裡初始化
    m_promptText(font), m_txtYes(font), m_txtNo(font)
{
    m_bg.setSize({ static_cast<float>(Config::WINDOW_WIDTH), BAR_HEIGHT });
    m_bg.setPosition({ 0.f, static_cast<float>(Config::WINDOW_HEIGHT) - BAR_HEIGHT });
    m_bg.setFillColor(sf::Color(30, 30, 30));
    m_bg.setOutlineColor(sf::Color::White);
    m_bg.setOutlineThickness(2.f);

    m_infoText.setCharacterSize(18);
    m_infoText.setFillColor(sf::Color::Yellow);
    m_infoText.setPosition({ 20.f, static_cast<float>(Config::WINDOW_HEIGHT) - BAR_HEIGHT + 10.f });

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
        btn.shape.setSize({ btnSize, btnSize });
        btn.shape.setPosition({ startX + i * (btnSize + gap), yPos });
        btn.shape.setFillColor(sf::Color(60, 60, 60));
        btn.shape.setOutlineThickness(2.f);
        btn.shape.setOutlineColor(info.color);

        btn.label.setString(info.name);
        btn.label.setCharacterSize(14);
        btn.label.setFillColor(info.color);
        btn.label.setPosition({ btn.shape.getPosition().x + 5.f, btn.shape.getPosition().y + 5.f });

        btn.subLabel.setCharacterSize(14);
        btn.subLabel.setFillColor(sf::Color::White);
        btn.subLabel.setPosition({ btn.shape.getPosition().x + 5.f, btn.shape.getPosition().y + 65.f });

        m_buttons.push_back(std::move(btn));
        i++;
    }
}

void GameUI::initPrompt() {
    float w = 600.f;
    float h = 300.f;
    m_promptBg.setSize({ w, h });
    m_promptBg.setOrigin({ w / 2, h / 2 });
    m_promptBg.setPosition({ Config::WINDOW_WIDTH / 2.f, Config::WINDOW_HEIGHT / 2.f });
    m_promptBg.setFillColor(sf::Color(20, 20, 20, 240));
    m_promptBg.setOutlineThickness(3.f);
    m_promptBg.setOutlineColor(GOLD_COLOR);

    m_promptText.setString("Spend ALL GOLD to buy\n'God of Gamblers'?\n(Requires 100,000 Gold)");
    m_promptText.setCharacterSize(30);
    m_promptText.setFillColor(sf::Color::White);

    // [修正] SFML 3.0: size.x / size.y
    sf::FloatRect textBounds = m_promptText.getLocalBounds();
    m_promptText.setOrigin({ textBounds.size.x / 2, textBounds.size.y / 2 });
    m_promptText.setPosition({ m_promptBg.getPosition().x, m_promptBg.getPosition().y - 50.f });

    // Yes Button
    m_btnYes.setSize({ 150.f, 60.f });
    m_btnYes.setOrigin({ 75.f, 30.f });
    m_btnYes.setPosition({ m_promptBg.getPosition().x - 100.f, m_promptBg.getPosition().y + 80.f });
    m_btnYes.setFillColor(sf::Color(0, 150, 0));

    m_txtYes.setString("YES");
    m_txtYes.setCharacterSize(24);
    sf::FloatRect yb = m_txtYes.getLocalBounds();
    m_txtYes.setOrigin({ yb.size.x / 2, yb.size.y / 2 });
    m_txtYes.setPosition(m_btnYes.getPosition());

    // No Button
    m_btnNo.setSize({ 150.f, 60.f });
    m_btnNo.setOrigin({ 75.f, 30.f });
    m_btnNo.setPosition({ m_promptBg.getPosition().x + 100.f, m_promptBg.getPosition().y + 80.f });
    m_btnNo.setFillColor(sf::Color(150, 0, 0));

    m_txtNo.setString("NO");
    m_txtNo.setCharacterSize(24);
    sf::FloatRect nb = m_txtNo.getLocalBounds();
    m_txtNo.setOrigin({ nb.size.x / 2, nb.size.y / 2 });
    m_txtNo.setPosition(m_btnNo.getPosition());
}

void GameUI::updateButtons(bool isShopPhase) {
    if (m_playerStats.hasGambler) {
        bool hasGamblerBtn = false;
        for (auto& btn : m_buttons) if (btn.type == TowerType::Gambler) hasGamblerBtn = true;

        if (!hasGamblerBtn) {
            Button btn(m_font);
            btn.type = TowerType::Gambler;
            float btnSize = 90.f;
            float gap = 10.f;
            float startX = 250.f + 6 * (btnSize + gap);
            float yPos = static_cast<float>(Config::WINDOW_HEIGHT) - BAR_HEIGHT + 15.f;

            btn.shape.setSize({ btnSize, btnSize });
            btn.shape.setPosition({ startX, yPos });
            btn.shape.setFillColor(sf::Color::Black);
            btn.shape.setOutlineThickness(2.f);
            btn.shape.setOutlineColor(GOLD_COLOR);

            btn.label.setString("GOD");
            btn.label.setPosition({ btn.shape.getPosition().x + 5.f, btn.shape.getPosition().y + 5.f });
            btn.label.setFillColor(GOLD_COLOR);

            btn.subLabel.setString("x1");
            btn.subLabel.setPosition({ btn.shape.getPosition().x + 5.f, btn.shape.getPosition().y + 65.f });

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
            }
            else {
                btn.subLabel.setString(std::format("x{}", count));
                btn.subLabel.setFillColor(sf::Color::White);
            }
            continue;
        }

        if (m_playerStats.level < info.requiredLevel) {
            btn.locked = true;
            btn.shape.setFillColor(sf::Color(100, 30, 30));
            btn.subLabel.setString(std::format("Lv.{}", info.requiredLevel));
            btn.subLabel.setFillColor(sf::Color::Red);
        }
        else {
            btn.locked = false;
            btn.shape.setFillColor(sf::Color(60, 60, 60));

            if (isShopPhase) {
                btn.subLabel.setString(std::format("${}", info.price));
                if (m_playerStats.gold < info.price)
                    btn.subLabel.setFillColor(sf::Color::Red);
                else
                    btn.subLabel.setFillColor(sf::Color::Green);
            }
            else {
                int count = m_playerStats.inventory[btn.type];
                btn.subLabel.setString(std::format("x{}", count));
                btn.subLabel.setFillColor(count > 0 ? sf::Color::White : sf::Color(150, 150, 150));
            }
        }
    }
}

void GameUI::draw(sf::RenderWindow& window, bool isShopPhase) {
    window.draw(m_bg);
    std::string stateStr = isShopPhase ? "[SHOP PHASE]" : "[WAVE INCOMING]";
    std::string info = std::format("{}\nGold: {}\nLives: {}\nLevel: {} Stars",
        stateStr, m_playerStats.gold, m_playerStats.lives, m_playerStats.level);
    m_infoText.setString(info);
    window.draw(m_infoText);

    updateButtons(isShopPhase);
    for (const auto& btn : m_buttons) {
        window.draw(btn.shape);
        window.draw(btn.label);
        window.draw(btn.subLabel);
    }
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