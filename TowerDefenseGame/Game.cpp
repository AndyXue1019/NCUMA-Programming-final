#include "Game.hpp"

#include <format>
#include <iostream>

#include "Config.hpp"
#include "GameUI.hpp"
#include "Map.hpp"
#include "Towers.hpp"
#include "Utils.hpp"

Game::Game()
    : m_window(sf::VideoMode({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}), "Tower Defense Game"),
      m_map(sf::Vector2u(static_cast<unsigned>(Config::WINDOW_WIDTH / Config::GRID_SIZE),
                         static_cast<unsigned>(Config::WINDOW_HEIGHT / Config::GRID_SIZE)),
            static_cast<float>(Config::GRID_SIZE)),
      m_uiText(m_font),
      m_videoSprite(m_videoTexture),
      m_finishSprite(m_finishTexture),
      m_winSprite(m_winTexture),
      m_titleText(m_font),
      m_rulesContent(m_font),
      m_btnStart(m_font),
      m_btnRules(m_font),
      m_btnRuleStart(m_font) {
    m_window.setFramerateLimit(Config::FRAME_RATE_LIMIT);

    loadResources();
    initMenu();
    initTestPath();
    m_map.addPath(m_testPath);

    m_gameState = GameState::MainMenu;
    m_gameUI = std::make_unique<GameUI>(m_font, m_playerStats);
    m_upgradePanel = std::make_unique<UpgradePanel>(m_font);
    m_inventoryPanel = std::make_unique<InventoryPanel>(m_font, m_playerStats);

    m_waveManager = std::make_unique<WaveManager>(m_enemies, m_testPath, m_playerStats);

    // 載入影片截圖 ( GodOfGamblers.jpg)
    // DEBUG
    if (!m_videoTexture.loadFromFile("GodOfGamblers.jpg")) {
        std::cout << "Image NOT found! Creating RED fallback." << std::endl;
        sf::Image img;
        img.resize({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}, sf::Color::Red);
        bool loaded = m_videoTexture.loadFromImage(img);
        if (!loaded) {
            std::cerr << "Fallback image creation failed!" << std::endl;
        }
    } else {
        std::cout << "Image loaded successfully!" << std::endl;
    }

    m_videoSprite.setTexture(m_videoTexture);

    sf::Vector2u size = m_videoTexture.getSize();
    if (size.x > 0 && size.y > 0) {
        m_videoSprite.setScale({static_cast<float>(Config::WINDOW_WIDTH) / size.x,
                                static_cast<float>(Config::WINDOW_HEIGHT) / size.y});
        m_videoSprite.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(size.x), static_cast<int>(size.y)}));
    }

    if (!m_finishTexture.loadFromFile("finish.png")) {
        std::cout << "Failed to load finish.png! Creating fallback." << std::endl;
        sf::Image img;
        img.resize({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}, sf::Color::Red);
        bool loaded = m_finishTexture.loadFromImage(img);
        if (!loaded) {
            std::cerr << "Fallback image creation failed!" << std::endl;
        }
    }

    // 2. 取得這張圖片專屬的大小
    sf::Vector2u finishSize = m_finishTexture.getSize();

    m_finishSprite.setTexture(m_finishTexture);
    if (finishSize.x > 0 && finishSize.y > 0) {
        m_finishSprite.setTextureRect(sf::IntRect(
            {0, 0},
            {static_cast<int>(finishSize.x), static_cast<int>(finishSize.y)}));

        float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / static_cast<float>(finishSize.x);
        float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / static_cast<float>(finishSize.y);
        m_finishSprite.setScale({scaleX, scaleY});
    }

    if (!m_winTexture.loadFromFile("win.png")) {
        std::cout << "Failed to load win.png! Creating fallback." << std::endl;
        sf::Image img;
        img.resize({Config::WINDOW_WIDTH, Config::WINDOW_HEIGHT}, sf::Color::Green);
        if (!m_winTexture.loadFromImage(img)) {
            std::cerr << "Fallback image creation failed!" << std::endl;
        }
    }

    m_winSprite.setTexture(m_winTexture);
    sf::Vector2u winSize = m_winTexture.getSize();
    if (winSize.x > 0 && winSize.y > 0) {
        m_winSprite.setTextureRect(sf::IntRect(
            {0, 0},
            {static_cast<int>(winSize.x), static_cast<int>(winSize.y)}));

        float scaleX = static_cast<float>(Config::WINDOW_WIDTH) / static_cast<float>(winSize.x);
        float scaleY = static_cast<float>(Config::WINDOW_HEIGHT) / static_cast<float>(winSize.y);
        m_winSprite.setScale({scaleX, scaleY});
    }
}

void Game::loadResources() {
    if (!m_font.openFromFile("C://Windows/Fonts/arial.ttf")) {
        if (!m_font.openFromFile("arial.ttf")) {
            std::cerr << "Failed to load font!" << std::endl;
        }
    }
    m_uiText.setFont(m_font);
    m_uiText.setCharacterSize(20);
    m_uiText.setFillColor(sf::Color::White);
    m_uiText.setPosition({10.f, 10.f});
}

void Game::initTestPath() {
    m_testPath.push_back({0.f, 100.f});
    m_testPath.push_back({300.f, 100.f});
    m_testPath.push_back({300.f, 500.f});
    m_testPath.push_back({800.f, 500.f});
    m_testPath.push_back({800.f, 200.f});
    m_testPath.push_back({1280.f, 200.f});
}

// 實作檢查事件
void Game::checkGamblerEvent() {
    if (m_playerStats.gold >= 100000 && !m_playerStats.hasTriggeredGamblerEvent) {
        m_gameState = GameState::GamblerEventPrompt;
        m_playerStats.hasTriggeredGamblerEvent = true;
    }
}

// 實作變身邏輯
void Game::triggerGamblerTransformation() {
    m_playerStats.gold = 0;

    m_towers.clear();
    m_selectedTower = std::nullopt;
    if (m_upgradePanel) m_upgradePanel->setSelectedTower(nullptr);

    m_enemies.clear();
    m_projectiles.clear();
    m_loots.clear();

    m_waveManager->forceEndWave();

    m_playerStats.hasGambler = true;
    m_playerStats.inventory[TowerType::Gambler] = 1;

    m_gameState = GameState::GamblerVideoPlaying;
    m_videoTimer = 0.f;
}

void Game::processEvents() {
    while (const auto event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
            m_isRunning = false;
        }

        if (m_gameState == GameState::MainMenu) {
            if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(static_cast<float>(mouseButton->position.x), static_cast<float>(mouseButton->position.y));

                    if (m_btnStart.isClicked(mousePos)) {
                        m_gameState = GameState::Shop;  // 開始遊戲 (進入商店階段)
                    } else if (m_btnRules.isClicked(mousePos)) {
                        m_gameState = GameState::Rules;  // 進入規則頁面
                    }
                }
            }
            continue;  // 如果是選單事件，處理完就換下一個 event，不跑下面的邏輯
        }

        if (m_gameState == GameState::Rules) {
            if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(static_cast<float>(mouseButton->position.x), static_cast<float>(mouseButton->position.y));

                    if (m_btnRuleStart.isClicked(mousePos)) {
                        m_gameState = GameState::Shop;  // 從規則頁面直接開始
                    }
                }
            }
            continue;
        }

        // 詢問視窗的點擊
        if (m_gameState == GameState::GamblerEventPrompt) {
            if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseButton->button == sf::Mouse::Button::Left) {
                    sf::Vector2f mousePos(static_cast<float>(mouseButton->position.x), static_cast<float>(mouseButton->position.y));
                    int result = m_gameUI->handlePromptClick(mousePos);
                    if (result == 1) {  // YES
                        triggerGamblerTransformation();
                    } else if (result == 0) {  // NO
                        m_gameState = GameState::WaveRunning;
                    }
                }
            }
            return;
        }

        if (m_gameState == GameState::GamblerVideoPlaying) {
            return;
        }

        if (const auto* mouseMove = event->getIf<sf::Event::MouseMoved>()) {
            sf::Vector2f mousePos(static_cast<float>(mouseMove->position.x), static_cast<float>(mouseMove->position.y));
            if (m_inventoryPanel) {
                m_inventoryPanel->handleMouseMove(mousePos);
            }
        } else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
            if (keyPressed->code == sf::Keyboard::Key::Enter) {
                if (m_gameState == GameState::Shop && m_playerStats.currentWave < 20) {
                    m_gameState = GameState::WaveRunning;
                    m_waveManager->startNextWave();
                    m_selectedTower = std::nullopt;

                    std::string title = "WAVE " + std::to_string(m_playerStats.currentWave);

                    spawnFloatingText(
                        title,
                        {Config::WINDOW_WIDTH / 2.f - 50.0f, Config::WINDOW_HEIGHT / 2.f - 50.0f},
                        sf::Color::Red,  // 顏色
                        80,              // 大小：80 (很大)
                        2.5f,            // 時間：2.5 秒 (淡出較慢)
                        {0.f, 0.f}       // 速度：{0,0} (靜止不動)
                    );
                }
            }
        } else if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseButton->button == sf::Mouse::Button::Left) {
                sf::Vector2f mousePos(static_cast<float>(mouseButton->position.x), static_cast<float>(mouseButton->position.y));

                if (m_inventoryPanel) {
                    bool handled = m_inventoryPanel->handleMousePress(mousePos);
                    if (handled)
                        return;
                    else
                        m_inventoryPanel->close();
                }

                if (m_upgradePanel->handleClick(mousePos, m_playerStats)) {
                    return;
                }

                if (mousePos.y > Config::WINDOW_HEIGHT - GameUI::BAR_HEIGHT) {
                    handleShopClick(mousePos);
                } else {
                    handleMapClick(mousePos);
                }
            } else if (mouseButton->button == sf::Mouse::Button::Right) {
                m_selectedTower = std::nullopt;
                m_upgradePanel->setSelectedTower(nullptr);
            }
        } else if (const auto* mouseReleased = event->getIf<sf::Event::MouseButtonReleased>()) {
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

    // 取得目前該塔的庫存
    int currentStock = m_playerStats.inventory[type];

    // [修正] 商店階段的邏輯優化
    if (m_gameState == GameState::Shop) {
        // 特例：如果是賭神塔，且我們已經擁有它 -> 視為「選取」而不是購買
        if (type == TowerType::Gambler && m_playerStats.hasGambler) {
            m_selectedTower = type;
            std::cout << "Selected God Tower for placement." << std::endl;
            return;
        }

        // 一般塔的購買邏輯
        // 如果是點擊一般塔，且錢夠 -> 購買
        if (m_playerStats.gold >= info.price) {
            m_playerStats.gold -= info.price;
            m_playerStats.inventory[type]++;
        } else {
            std::cout << "Not enough gold!" << std::endl;
        }

        // (選用優化) 如果你希望一般塔買了之後直接選取，可以加這行：
        // if (m_playerStats.inventory[type] > 0) m_selectedTower = type;
    }
    // 戰鬥階段的邏輯 (維持不變)
    else if (m_gameState == GameState::WaveRunning) {
        if (currentStock > 0) {
            m_selectedTower = type;
        }
    }
}
void Game::handleMapClick(sf::Vector2f mousePos) {
    if (m_selectedTower) {
        TowerType type = *m_selectedTower;
        if (m_playerStats.inventory[type] > 0) {
            for (const auto& t : m_towers) {
                if (t->getBounds().contains(mousePos)) return;
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
                case TowerType::Gambler:
                    newTower = std::make_unique<GamblerTower>(mousePos, m_enemies, m_projectiles, m_playerStats);
                    break;
            }

            if (newTower) {
                newTower->setTextCallback([this](std::string text, sf::Vector2f pos, sf::Color color) {
                    this->spawnFloatingText(text, pos, color);
                });

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
    if (m_gameState == GameState::GameOver) {
        return;
    }

    if (m_gameState == GameState::MainMenu || m_gameState == GameState::Rules) {
        updateMenu();
        return;  // 選單狀態下不更新遊戲邏輯
    }

    //  檢查玩家血量是否歸零
    if (m_playerStats.lives <= 0) {
        m_gameState = GameState::GameOver;
        std::cout << "Game Over triggered!" << std::endl;
        return;
    }

    if (m_gameState == GameState::WaveRunning || m_gameState == GameState::Shop) {
        checkGamblerEvent();
    }

    if (m_gameState == GameState::GamblerEventPrompt) return;

    if (m_gameState == GameState::GamblerVideoPlaying) {
        m_videoTimer += dt.asSeconds();
        if (m_videoTimer >= m_videoDuration) {
            m_gameState = GameState::Shop;
        }
        return;
    }

    if (m_gameState == GameState::WaveRunning) {
        m_waveManager->update(dt);
        if (!m_waveManager->isWaveInProgress()) {
            if (m_playerStats.currentWave >= 20) {
                m_gameState = GameState::GameOver;  // 遊戲結束
                std::cout << "Victory! Reached wave 20." << std::endl;
            } else {
                // 還沒到 20 波，進入商店階段
                m_gameState = GameState::Shop;
            }
        }
    }
    m_upgradePanel->update(dt);
    if (m_inventoryPanel) m_inventoryPanel->update(dt);

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
                        static_cast<AccessoryType>(Utils::m_rand() % 5 + 1)));
                }
            } else {
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

    // 更新漂浮文字
    for (auto it = m_floatingTexts.begin(); it != m_floatingTexts.end();) {
        it->lifeTime -= dt.asSeconds();
        it->text.move(it->velocity * dt.asSeconds());

        // 淡出效果
        sf::Color c = it->text.getFillColor();
        c.a = static_cast<std::uint8_t>(255 * std::max(0.f, it->lifeTime));
        it->text.setFillColor(c);

        if (it->lifeTime <= 0.f)
            it = m_floatingTexts.erase(it);
        else
            ++it;
    }

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

    for (const auto& ft : m_floatingTexts) m_window.draw(ft.text);

    // 繪製選單
    if (m_gameState == GameState::MainMenu) {
        m_window.draw(m_titleText);
        m_btnStart.draw(m_window);
        m_btnRules.draw(m_window);
    }
    // 繪製規則頁面
    else if (m_gameState == GameState::Rules) {
        m_window.draw(m_rulesContent);
        m_btnRuleStart.draw(m_window);
    } else if (m_gameState == GameState::GamblerVideoPlaying) {
        m_window.draw(m_videoSprite);
    } else if (m_gameState == GameState::GameOver) {
        if (m_playerStats.lives > 0) {
            m_window.draw(m_winSprite);
        } else {
            m_window.draw(m_finishSprite);
        }
    } else {
        m_map.draw(m_window, sf::RenderStates::Default);
        for (const auto& tower : m_towers) tower->draw(m_window);
        for (const auto& enemy : m_enemies) enemy->draw(m_window);
        for (const auto& proj : m_projectiles) proj->draw(m_window);
        for (const auto& loot : m_loots) loot->draw(m_window);
        m_gameUI->draw(m_window, (m_gameState == GameState::Shop));
        m_upgradePanel->draw(m_window, m_playerStats);
        m_inventoryPanel->draw(m_window);

        if (m_gameState == GameState::GamblerEventPrompt) {
            sf::RectangleShape mask({(float)Config::WINDOW_WIDTH, (float)Config::WINDOW_HEIGHT});
            mask.setFillColor(sf::Color(0, 0, 0, 150));
            m_window.draw(mask);
            m_gameUI->drawGamblerPrompt(m_window);
        }

        if (m_selectedTower) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(m_window);
            sf::RectangleShape preview({40.f, 40.f});
            preview.setOrigin({20.f, 20.f});
            preview.setPosition({static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)});
            preview.setFillColor(sf::Color(255, 255, 255, 100));
            m_window.draw(preview);
        }
    }
    m_window.display();
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

void Game::spawnFloatingText(const std::string& str, sf::Vector2f pos, sf::Color color, int size, float lifeTime, sf::Vector2f velocity) {
    FloatingText ft(m_font, lifeTime, velocity);

    ft.text.setString(str);
    ft.text.setCharacterSize(size);
    ft.text.setFillColor(color);
    ft.text.setOutlineColor(sf::Color::Black);
    ft.text.setOutlineThickness(2.f);

    sf::FloatRect bounds = ft.text.getLocalBounds();
    ft.text.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});  // 確保置中
    ft.text.setPosition(pos);

    m_floatingTexts.push_back(std::move(ft));
}

void Game::initMenu() {
    // 1. 設定主標題
    std::string titleStr = "Normal Tower Defence Game";
    m_titleText.setString(sf::String::fromUtf8(titleStr.begin(), titleStr.end()));
    m_titleText.setCharacterSize(60);
    m_titleText.setFillColor(sf::Color::White);
    m_titleText.setStyle(sf::Text::Bold);

    sf::FloatRect bounds = m_titleText.getLocalBounds();
    m_titleText.setOrigin({bounds.size.x / 2.f, bounds.size.y / 2.f});
    m_titleText.setPosition({static_cast<float>(Config::WINDOW_WIDTH) / 2.f, 150.f});

    // 2. 設定按鈕
    float centerX = static_cast<float>(Config::WINDOW_WIDTH) / 2.f;
    float startY = 350.f;
    float gap = 100.f;

    // 這裡 init 不再需要傳 m_font
    m_btnStart.init("Start", {centerX, startY}, {200.f, 60.f});
    m_btnRules.init("Rules", {centerX, startY + gap}, {200.f, 60.f});

    // 3. 設定規則頁面內容
    m_rulesContent.setString("Game Rules:\n\n1. Build towers to stop enemies.\n2. Collect gold to buy the 'God of Gamblers'.\n3. Survive 20 waves.");
    m_rulesContent.setCharacterSize(24);
    m_rulesContent.setFillColor(sf::Color(200, 200, 200));
    m_rulesContent.setPosition({100.f, 100.f});

    // 4. 設定規則頁面按鈕
    m_btnRuleStart.init("Start",
                        {static_cast<float>(Config::WINDOW_WIDTH) - 150.f, static_cast<float>(Config::WINDOW_HEIGHT) - 100.f},
                        {150.f, 50.f});
}

void Game::updateMenu() {
    // 取得滑鼠座標
    sf::Vector2i pixelPos = sf::Mouse::getPosition(m_window);
    sf::Vector2f mousePos = m_window.mapPixelToCoords(pixelPos);

    if (m_gameState == GameState::MainMenu) {
        m_btnStart.update(mousePos);
        m_btnRules.update(mousePos);
    } else if (m_gameState == GameState::Rules) {
        m_btnRuleStart.update(mousePos);
    }
}