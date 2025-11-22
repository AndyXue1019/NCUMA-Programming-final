#include "Config.hpp"
#include "Game.hpp"
#include "Towers.hpp"
#include "Map.hpp"
#include <format> // C++20 for std::format
#include <iostream>


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
        }
        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            // 按下 Enter 開始下一波
            if (keyPressed->code == sf::Keyboard::Key::Enter) {
                m_waveManager->startNextWave();
            }
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
                if (m_playerStats.gold >= 50) {
                    sf::Vector2f pos(static_cast<float>(mouseButton->position.x), static_cast<float>(mouseButton->position.y));

                    std::unique_ptr<Tower> newTower;
                    switch (m_selectedTowerType) {
                        case TowerType::Basic:
                            newTower = std::make_unique<BasicTower>(pos, m_enemies, m_projectiles);
                            break;
                        case TowerType::Laser:
                            newTower = std::make_unique<LaserTower>(pos, m_enemies, m_projectiles);
                            break;
                        case TowerType::Sniper:
                            newTower = std::make_unique<SniperTower>(pos, m_enemies, m_projectiles);
                            break;
                        case TowerType::Slow:
                            newTower = std::make_unique<SlowTower>(pos, m_enemies, m_projectiles);
                            break;
                        case TowerType::Teleport:
                            newTower = std::make_unique<TeleportTower>(pos, m_enemies, m_projectiles);
                            break;
                        case TowerType::SelfDestruct:
                            newTower = std::make_unique<SelfDestructTower>(pos, m_enemies, m_projectiles);
                            break;
                    }

                    if (newTower && m_playerStats.gold >= newTower->getPrice()) {
                        m_playerStats.gold -= newTower->getPrice();
                        m_towers.push_back(std::move(newTower));
                    } else {
                        std::cout << "Not enough gold!" << std::endl;
                    }
                } else
                    std::cout << "Not enough gold to build any tower!" << std::endl;
            } else if (mouseButton->button == sf::Mouse::Button::Right) {
                // 右鍵賣掉塔 反還一半金幣
                sf::Vector2f pos(static_cast<float>(mouseButton->position.x), static_cast<float>(mouseButton->position.y));
                for (auto it = m_towers.begin(); it != m_towers.end(); ++it) {
                    if (Utils::distance((*it)->getPosition(), pos) <= Config::GRID_SIZE / 2.f) {
                        int sellPrice = (*it)->getPrice() / 2;
                        m_playerStats.gold += sellPrice;
                        m_towers.erase(it);
                        std::cout << "Sold tower for " << sellPrice << " gold." << std::endl;
                        break;
                    }
                }
            }
        }
    }
}

void Game::update(sf::Time dt) {
    // 1. 更新波次管理器
    m_waveManager->update(dt);

    // 2. 更新實體
    for (auto& enemy : m_enemies) enemy->update(dt);
    for (auto& tower : m_towers) tower->update(dt);
    for (auto& proj : m_projectiles) proj->update(dt);

    // 3. 清理與結算 (Garbage Collection & Economy)
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
    // 畫地圖
    m_window.clear(sf::Color::Black);
    m_map.draw(m_window, sf::RenderStates::Default);
    // 畫防禦塔
    for (const auto& tower : m_towers) tower->draw(m_window);
    // 畫敵人
    for (const auto& enemy : m_enemies) enemy->draw(m_window);
    // 畫子彈
    for (const auto& proj : m_projectiles) proj->draw(m_window);
    
    // 繪製 UI
    m_window.draw(m_uiText);
    
    m_window.display();
}