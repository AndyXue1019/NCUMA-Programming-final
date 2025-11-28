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

    m_upgradePanel = std::make_unique<UpgradePanel>(m_font);
    
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
        }
        else if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButton->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos(static_cast<float>(mouseButton->position.x), static_cast<float>(mouseButton->position.y));

                // 1. 先檢查是否點到升級面板的按鈕 (優先權最高)
                if (m_upgradePanel->handleClick(mousePos, m_playerStats)) {
                    return; // 如果點了 UI，就不做其他事
                }

                // 2. 判斷點擊 UI 還是 地圖
                if (mousePos.y > Config::WINDOW_HEIGHT - GameUI::BAR_HEIGHT) {
                    handleShopClick(mousePos);
                }
                else {
                    handleMapClick(mousePos);
                }
            }
            // 右鍵取消選取 / 關閉面板
            else if (mouseButton->button == sf::Mouse::Button::Right) {
                m_selectedTower = std::nullopt;
                m_upgradePanel->setSelectedTower(nullptr); // 關閉面板
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
    // -------------------------------------------------
    // 情況 A: 建造模式 (手上已經選了要蓋的塔 m_selectedTower)
    // -------------------------------------------------
    if (m_selectedTower) {
        TowerType type = *m_selectedTower;

        // 1. 檢查庫存是否足夠
        if (m_playerStats.inventory[type] > 0) {

            // 2. (選用) 簡單碰撞檢查：不要蓋在其他塔上面
            for (const auto& t : m_towers) {
                if (t->getBounds().contains(mousePos)) {
                    std::cout << "Cannot place tower here! Space occupied." << std::endl;
                    return;
                }
            }

            // 3. 根據類型建立防禦塔
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

            // 4. 放置成功後的處理
            if (newTower) {
                m_towers.push_back(std::move(newTower));

                // 扣除庫存
                m_playerStats.inventory[type]--;

                // 放置新塔時，強制關閉升級面板，避免混淆
                if (m_upgradePanel) {
                    m_upgradePanel->setSelectedTower(nullptr);
                }

                std::cout << "Tower placed. Remaining stock: " << m_playerStats.inventory[type] << std::endl;

                // 如果該種塔庫存歸零，取消手中的選取狀態
                if (m_playerStats.inventory[type] <= 0) {
                    m_selectedTower = std::nullopt;
                    std::cout << "Out of stock, selection cleared." << std::endl;
                }
            }
        }
        else {
            // 理論上 UI 會擋，但雙重保險
            std::cout << "No inventory for this tower!" << std::endl;
            m_selectedTower = std::nullopt;
        }
        return; // 建造模式結束，不執行下方的選取邏輯
    }

    // -------------------------------------------------
    // 情況 B: 選取模式 (手上沒塔，點擊地圖上的塔來查看/升級)
    // -------------------------------------------------
    bool clickedTower = false;

    // 遍歷所有塔，檢查滑鼠是否點在塔的範圍內
    for (const auto& tower : m_towers) {
        if (tower->getBounds().contains(mousePos)) {
            // 找到了被點擊的塔
            if (m_upgradePanel) {
                // 通知 UI 面板顯示這座塔的資訊
                m_upgradePanel->setSelectedTower(tower.get());
            }

            clickedTower = true;
            std::cout << "Selected tower: " << tower->getName()
                << " (Lv." << tower->getLevel() << ")" << std::endl;

            // 選到一個就跳出迴圈 (避免重疊時一次選多個)
            break;
        }
    }

    // 如果點擊了地圖空白處 (沒點到任何塔)，則關閉面板
    if (!clickedTower) {
        if (m_upgradePanel) {
            m_upgradePanel->setSelectedTower(nullptr);
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

    m_upgradePanel->update(dt);

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
    m_upgradePanel->draw(m_window); //畫升級面板

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