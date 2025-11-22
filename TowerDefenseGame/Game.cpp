#include "Game.hpp"

#include <format>  // C++20 for std::format
#include <iostream>

#include "Config.hpp"
#include "Map.hpp"
#include "Towers.hpp"

Game::Game()
    : m_window(sf::VideoMode({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}), "Tower Defense Game"),
      // 建立 Map：格子數 = 視窗大小 / 每格像素 (Config::GRID_SIZE)
      m_map(sf::Vector2u(Config::WINDOW_WIDTH / Config::GRID_SIZE, Config::WINDOW_HEIGHT / Config::GRID_SIZE),
            static_cast<float>(Config::GRID_SIZE)),
      m_uiText(m_font),
      m_selectedTowerType(TowerType::Basic) {
    m_window.setFramerateLimit(Config::FRAME_RATE_LIMIT);

    loadResources();
    initTestPath();

    // 把測試路徑加入 Map，使路徑格子被標記並顯示為黃色
    m_map.addPath(m_testPath);

    m_gameUI = std::make_unique<GameUI>(m_font, m_playerStats);

    m_waveManager = std::make_unique<WaveManager>(m_enemies, m_testPath, m_playerStats);
}

void Game::loadResources() {
    if (!m_font.openFromFile("C://Windows/Fonts/arial.ttf")) {
        std::cerr << "Error: Could not load font arial.ttf" << std::endl;
    }
    m_uiText.setFont(m_font);
    m_uiText.setCharacterSize(20);
    m_uiText.setFillColor(sf::Color::White);
    m_uiText.setPosition({10.f, 10.f});
}

void Game::initTestPath() {
    // 設定幾個路徑點 (Waypoints)
    m_testPath.push_back({0.f, 100.f});
    m_testPath.push_back({300.f, 100.f});
    m_testPath.push_back({300.f, 500.f});
    m_testPath.push_back({800.f, 500.f});
    m_testPath.push_back({800.f, 200.f});
    m_testPath.push_back({1280.f, 200.f});
}

void Game::run() {
    sf::Clock clock;

    // Main Game Loop
    while (m_window.isOpen() && m_isRunning) {
        // 計算 Delta Time
        sf::Time dt = clock.restart();

        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    // SFML 3 事件處理方式
    while (const auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            m_isRunning = false;
        } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            // 按下 Enter 開始下一波
            if (keyPressed->code == sf::Keyboard::Key::Enter) {
                if (keyPressed->code == sf::Keyboard::Key::Enter) {
                    if (m_gameState == GameState::Shop && m_playerStats.currentWave < 20) {
                        m_gameState = GameState::WaveRunning;
                        m_waveManager->startNextWave();
                        // 清除手上選中的塔
                        m_selectedTower = std::nullopt;
                    }
                }
            }
            // debug用 按數字鍵選擇塔種類
            if (keyPressed->code == sf::Keyboard::Key::Num1) {
                m_selectedTowerType = TowerType::Basic;
                std::cout << "Selected: Basic" << std::endl;
            }
            if (keyPressed->code == sf::Keyboard::Key::Num2) {
                m_selectedTowerType = TowerType::Laser;
                std::cout << "Selected: Laser" << std::endl;
            }
            if (keyPressed->code == sf::Keyboard::Key::Num3) {
                m_selectedTowerType = TowerType::Sniper;
                std::cout << "Selected: Sniper" << std::endl;
            }
            if (keyPressed->code == sf::Keyboard::Key::Num4) {
                m_selectedTowerType = TowerType::Slow;
                std::cout << "Selected: Slow" << std::endl;
            }
            if (keyPressed->code == sf::Keyboard::Key::Num5) {
                m_selectedTowerType = TowerType::Teleport;
                std::cout << "Selected: Teleport" << std::endl;
            }
            if (keyPressed->code == sf::Keyboard::Key::Num6) {
                m_selectedTowerType = TowerType::SelfDestruct;
                std::cout << "Selected: Mine" << std::endl;
            }
        } else if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButton->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos(static_cast<float>(mouseButton->position.x), static_cast<float>(mouseButton->position.y));

                // 判斷點擊位置是 UI 還是 地圖
                // 假設 UI 在底部 BAR_HEIGHT 高度
                if (mousePos.y > Config::WINDOW_HEIGHT - GameUI::BAR_HEIGHT) {
                    handleShopClick(mousePos);  // 點擊 UI 區域
                } else {
                    handleMapClick(mousePos);  // 點擊地圖區域
                }
            }
            // 右鍵取消選取
            else if (mouseButton->button == sf::Mouse::Button::Right) {
                m_selectedTower = std::nullopt;
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
        // --- 商店階段：購買 ---
        if (m_playerStats.gold >= info.price) {
            m_playerStats.gold -= info.price;
            m_playerStats.inventory[type]++;
            std::cout << "Bought " << info.name << ". Inventory: " << m_playerStats.inventory[type] << std::endl;
        } else {
            std::cout << "Not enough gold!" << std::endl;
        }
    } else if (m_gameState == GameState::WaveRunning) {
        // --- 戰鬥階段：選擇塔 ---
        if (m_playerStats.inventory[type] > 0) {
            m_selectedTower = type;
            std::cout << "Selected " << info.name << " for placement." << std::endl;
        } else {
            std::cout << "No inventory!" << std::endl;
        }
    }
}

void Game::handleMapClick(sf::Vector2f mousePos) {
    // 只有在戰鬥階段 (或你想允許商店階段佈陣也可以，看設計) 且有選中塔時才能放置
    // 根據您的要求：塔可在回合結束時購買，回合開始後(WaveRunning) 就無法購買，
    // 通常塔防遊戲允許隨時放置，只要手上有塔。

    if (!m_selectedTower) return;

    // TODO: 這裡應該檢查放置位置是否合法 (Grid System)
    // 目前先簡化為直接放置

    TowerType type = *m_selectedTower;

    // 再次檢查庫存 (雙重保險)
    if (m_playerStats.inventory[type] > 0) {
        std::unique_ptr<Tower> newTower;
        switch (type) {
            case TowerType::Basic:
                newTower = std::make_unique<BasicTower>(mousePos, m_enemies, m_projectiles);
                break;
            case TowerType::Laser:
                newTower = std::make_unique<LaserTower>(mousePos, m_enemies, m_projectiles);
                break;
            case TowerType::Sniper:
                newTower = std::make_unique<SniperTower>(mousePos, m_enemies, m_projectiles);
                break;
            case TowerType::Slow:
                newTower = std::make_unique<SlowTower>(mousePos, m_enemies, m_projectiles);
                break;
            case TowerType::Teleport:
                newTower = std::make_unique<TeleportTower>(mousePos, m_enemies, m_projectiles);
                break;
            case TowerType::SelfDestruct:
                newTower = std::make_unique<SelfDestructTower>(mousePos, m_enemies, m_projectiles);
                break;
        }

        if (newTower) {
            m_towers.push_back(std::move(newTower));
            // 扣除庫存
            m_playerStats.inventory[type]--;

            // 如果庫存歸零，取消選取
            if (m_playerStats.inventory[type] <= 0) {
                m_selectedTower = std::nullopt;
            }
        }
    }
}

void Game::update(sf::Time dt) {
    // 檢查波次是否結束，若結束則切回商店模式
    if (m_gameState == GameState::WaveRunning) {
        m_waveManager->update(dt);

        // 檢查條件：沒有生成中 且 場上沒敵人
        if (!m_waveManager->isWaveInProgress()) {
            m_gameState = GameState::Shop;
            std::cout << "Wave Cleared! Shopping Phase." << std::endl;
        }
    }

    // 更新實體
    for (auto& enemy : m_enemies) enemy->update(dt);
    for (auto& tower : m_towers) tower->update(dt);
    for (auto& proj : m_projectiles) proj->update(dt);

    // 清理與結算 (Garbage Collection & Economy)
    std::erase_if(m_enemies, [&](const auto& enemy) {
        if (!enemy->isActive()) {
            // 判斷是死亡還是到達終點
            if (enemy->isDead()) {                          // 假設 isActive 為 false 且 HP <= 0 為死亡
                m_playerStats.addGold(enemy->getBounty());  // 獲得金幣
                m_playerStats.addExp(enemy->getExp());      // 獲得經驗
                m_waveManager->notifyEnemyDeath();
            } else {
                // 到達終點，扣hp
                m_playerStats.lives--;
                m_waveManager->notifyEnemyReachedEnd();
            }
            return true;  // Remove
        }
        return false;  // Keep
    });
    // 清理已擊中或無效的子彈
    std::erase_if(m_projectiles, [](const auto& proj) {
        return !proj->isActive();
    });
    // 清理已銷毀的塔 (例如自爆塔)
    std::erase_if(m_towers, [](const auto& tower) {
        return !tower->isActive();
    });

    updateUI();
}

void Game::updateUI() {
    // C++20 std::format
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

    m_map.draw(m_window, sf::RenderStates::Default);              // 畫地圖
    for (const auto& tower : m_towers) tower->draw(m_window);     // 畫防禦塔
    for (const auto& enemy : m_enemies) enemy->draw(m_window);    // 畫敵人
    for (const auto& proj : m_projectiles) proj->draw(m_window);  // 畫子彈

    m_gameUI->draw(m_window, (m_gameState == GameState::Shop));  // 畫UI

    // Preview 放置中的塔
    if (m_selectedTower) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
        sf::RectangleShape preview({40.f, 40.f});
        preview.setOrigin({20.f, 20.f});
        preview.setPosition({static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)});
        preview.setFillColor(sf::Color(255, 255, 255, 100));  // 半透明
        m_window.draw(preview);
    }

    m_window.display();
}