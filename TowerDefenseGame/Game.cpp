#include "Game.hpp"

#include <format> 
#include <iostream>

#include "Config.hpp"
#include "Map.hpp"
#include "Towers.hpp"
#include "Utils.hpp"

Game::Game()
    : m_window(sf::VideoMode({ Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT }), "Tower Defense Game"),
    m_map(sf::Vector2u(static_cast<unsigned>(Config::WINDOW_WIDTH / Config::GRID_SIZE),
        static_cast<unsigned>(Config::WINDOW_HEIGHT / Config::GRID_SIZE)),
        static_cast<float>(Config::GRID_SIZE)),
    m_uiText(m_font) {
    m_window.setFramerateLimit(Config::FRAME_RATE_LIMIT);

    loadResources();
    initTestPath();
    m_map.addPath(m_testPath);

    m_gameUI = std::make_unique<GameUI>(m_font, m_playerStats);
    m_upgradePanel = std::make_unique<UpgradePanel>(m_font);
    m_inventoryPanel = std::make_unique<InventoryPanel>(m_font, m_playerStats);

    m_waveManager = std::make_unique<WaveManager>(m_enemies, m_testPath, m_playerStats);
}

void Game::loadResources() {
    if (!m_font.openFromFile("C://Windows/Fonts/arial.ttf")) {
        std::cerr << "Error: Could not load font arial.ttf" << std::endl;
    }
    m_uiText.setFont(m_font);
    m_uiText.setCharacterSize(20);
    m_uiText.setFillColor(sf::Color::White);
    m_uiText.setPosition({ 10.f, 10.f });

    // 移除圖片載入
}

void Game::initTestPath() {
    m_testPath.push_back({ 0.f, 100.f });
    m_testPath.push_back({ 300.f, 100.f });
    m_testPath.push_back({ 300.f, 500.f });
    m_testPath.push_back({ 800.f, 500.f });
    m_testPath.push_back({ 800.f, 200.f });
    m_testPath.push_back({ 1280.f, 200.f });
}

void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen() && m_isRunning) {
        sf::Time dt = clock.restart();
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    while (const auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            m_isRunning = false;
        }
        else if (const auto* mouseMove = event->getIf<sf::Event::MouseMoved>()) {
            sf::Vector2f mousePos(static_cast<float>(mouseMove->position.x), static_cast<float>(mouseMove->position.y));
            if (m_inventoryPanel) {
                m_inventoryPanel->handleMouseMove(mousePos);
            }
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Enter) {
                if (m_gameState == GameState::Shop && m_playerStats.currentWave < 20) {
                    m_gameState = GameState::WaveRunning;
                    m_waveManager->startNextWave();
                    m_selectedTower = std::nullopt;
                }
            }
        }
        else if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButton->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos(static_cast<float>(mouseButton->position.x), static_cast<float>(mouseButton->position.y));

                // -----------------------------------------------------------
                // 優先順序 1: 背包 UI
                // -----------------------------------------------------------
                if (m_inventoryPanel) {
                    // 嘗試處理背包點擊
                    bool handled = m_inventoryPanel->handleMousePress(mousePos);

                    if (handled) {
                        return; // 如果點在背包上，就結束，不傳給地圖
                    }
                    else {
                        // 點在背包外面 -> 自動關閉背包
                        m_inventoryPanel->close();
                    }
                }
                // -----------------------------------------------------------

                // 優先順序 2: 升級面板
                if (m_upgradePanel->handleClick(mousePos, m_playerStats)) {
                    return;
                }

                // 優先順序 3: 底部 UI 與 地圖
                if (mousePos.y > Config::WINDOW_HEIGHT - GameUI::BAR_HEIGHT) {
                    handleShopClick(mousePos);
                }
                else {
                    handleMapClick(mousePos);
                }
            }
            else if (mouseButton->button == sf::Mouse::Button::Right) {
                m_selectedTower = std::nullopt;
                m_upgradePanel->setSelectedTower(nullptr);
            }
        }
        // --- 新增：處理滑鼠放開 (用於結束拖曳) ---
        else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseReleased->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos(static_cast<float>(mouseReleased->position.x), static_cast<float>(mouseReleased->position.y));
                if (m_inventoryPanel) {
                    m_inventoryPanel->handleMouseRelease(mousePos);
                }
            }
        }
    }
}

void Game::handleShopClick(sf::Vector2f mousePos) {
    auto clickedType = m_gameUI->handleClick(mousePos);
    if (!clickedType) return;
    TowerType type = *clickedType;
    const auto& info = TowerData::INFO.at(type);

    if (m_gameState == GameState::Shop) {
        if (m_playerStats.gold >= info.price) {
            m_playerStats.gold -= info.price;
            m_playerStats.inventory[type]++;
        }
        else {
            std::cout << "Not enough gold!" << std::endl;
        }
    }
    else if (m_gameState == GameState::WaveRunning) {
        if (m_playerStats.inventory[type] > 0) {
            m_selectedTower = type;
        }
    }
}

void Game::handleMapClick(sf::Vector2f mousePos) {
    if (m_selectedTower) {
        TowerType type = *m_selectedTower;
        if (m_playerStats.inventory[type] > 0) {
            for (const auto& t : m_towers) {
                if (t->getBounds().contains(mousePos)) {
                    return;
                }
            }

            std::unique_ptr<Tower> newTower;
            switch (type) {
            case TowerType::Basic:
                newTower = std::make_unique<BasicTower>(mousePos, m_enemies, m_projectiles, m_playerStats);
                break;
            case TowerType::Laser:
                newTower = std::make_unique<LaserTower>(mousePos, m_enemies, m_projectiles, m_playerStats);
                break;
            case TowerType::Sniper:
                newTower = std::make_unique<SniperTower>(mousePos, m_enemies, m_projectiles, m_playerStats);
                break;
            case TowerType::Slow:
                newTower = std::make_unique<SlowTower>(mousePos, m_enemies, m_projectiles, m_playerStats);
                break;
            case TowerType::Teleport:
                newTower = std::make_unique<TeleportTower>(mousePos, m_enemies, m_projectiles, m_playerStats);
                break;
            case TowerType::SelfDestruct:
                newTower = std::make_unique<SelfDestructTower>(mousePos, m_enemies, m_projectiles, m_playerStats);
                break;
            }

            if (newTower) {
                m_towers.push_back(std::move(newTower));
                m_playerStats.inventory[type]--;
                if (m_upgradePanel) m_upgradePanel->setSelectedTower(nullptr);
                if (m_playerStats.inventory[type] <= 0) m_selectedTower = std::nullopt;
            }
        }
        return;
    }

    bool clickedTower = false;
    for (const auto& tower : m_towers) {
        if (tower->getBounds().contains(mousePos)) {
            if (m_upgradePanel) m_upgradePanel->setSelectedTower(tower.get());
            clickedTower = true;
            break;
        }
    }

    if (!clickedTower) {
        if (m_upgradePanel) m_upgradePanel->setSelectedTower(nullptr);
    }
}

void Game::update(sf::Time dt) {
    if (m_gameState == GameState::WaveRunning) {
        m_waveManager->update(dt);
        if (!m_waveManager->isWaveInProgress()) {
            m_gameState = GameState::Shop;
        }
    }
    m_upgradePanel->update(dt);

    if (m_inventoryPanel) {
        m_inventoryPanel->update(dt);
    }

    for (auto& enemy : m_enemies) enemy->update(dt);
    for (auto& tower : m_towers) tower->update(dt);
    for (auto& proj : m_projectiles) proj->update(dt);
    for (auto& loot : m_loots) loot->update(dt);

    std::erase_if(m_enemies, [&](const auto& enemy) {
        if (!enemy->isActive()) {
            if (enemy->isDead()) {
                m_playerStats.addGold(enemy->getBounty());
                m_playerStats.addExp(enemy->getExp());
                m_waveManager->notifyEnemyDeath();
                if (Utils::m_rand() < 5) {
                    m_loots.push_back(std::make_unique<Loot>(
                        enemy->getPosition(),
                        static_cast<AccessoryType>(Utils::m_rand() % 5 + 1)
                    ));
                }
            }
            else {
                m_playerStats.lives--;
                m_waveManager->notifyEnemyReachedEnd();
            }
            return true;
        }
        return false;
        });

    std::erase_if(m_loots, [&](const auto& loot) {
        if (loot->hasArrived()) {
            m_playerStats.accessories.push_back(loot->getType());
            return true;
        }
        return false;
        });
    std::erase_if(m_projectiles, [](const auto& proj) { return !proj->isActive(); });
    std::erase_if(m_towers, [](const auto& tower) { return !tower->isActive(); });

    updateUI();
}

void Game::updateUI() {
    std::string info = "Wave: " + std::to_string(m_playerStats.currentWave) + "/20" +
        "\nGold: " + std::to_string(m_playerStats.gold) +
        "\nLives: " + std::to_string(m_playerStats.lives) +
        "\nLevel: " + std::to_string(m_playerStats.level);

    if (!m_waveManager->isWaveInProgress() && m_playerStats.currentWave < 20) {
        info += "\nPress ENTER to start next wave";
    }
    m_uiText.setString(info);
}

void Game::render() {
    m_window.clear(sf::Color::Black);
    m_map.draw(m_window, sf::RenderStates::Default);
    for (const auto& tower : m_towers) tower->draw(m_window);
    for (const auto& enemy : m_enemies) enemy->draw(m_window);
    for (const auto& proj : m_projectiles) proj->draw(m_window);
    for (const auto& loot : m_loots) loot->draw(m_window);
    m_gameUI->draw(m_window, (m_gameState == GameState::Shop));
    m_upgradePanel->draw(m_window, m_playerStats);
    m_inventoryPanel->draw(m_window);

    if (m_selectedTower) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
        sf::RectangleShape preview({ 40.f, 40.f });
        preview.setOrigin({ 20.f, 20.f });
        preview.setPosition({ static_cast<float>(mousePos.x), static_cast<float>(mousePos.y) });
        preview.setFillColor(sf::Color(255, 255, 255, 100));
        m_window.draw(preview);
    }
    m_window.display();
}