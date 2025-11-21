#include "Tower.hpp"
#include "Utils.hpp"

Tower::Tower(sf::Vector2f pos, const std::vector<std::shared_ptr<Enemy>>& enemies, std::vector<std::unique_ptr<Projectile>>& projectiles)
    : m_enemies(enemies), m_projectiles(projectiles) {
    m_shape.setSize({40.f, 40.f});
    m_shape.setOrigin({20.f, 20.f});
    m_shape.setPosition(pos);
}

void Tower::update(sf::Time dt) {
    if (m_currentCooldown > 0.f) {
        m_currentCooldown -= dt.asSeconds();
    }

    // 如果冷卻完畢，嘗試執行動作
    if (m_currentCooldown <= 0.f) {
        performAction();
        // 注意：performAction 內部決定是否重置 m_currentCooldown
        // 因為有些塔 (如緩速) 是持續效果，有些是觸發效果
    }
}

void Tower::draw(sf::RenderWindow& window) {
    window.draw(m_shape);
}

std::shared_ptr<Enemy> Tower::findTarget(float range) {
    Enemy* bestTarget = nullptr;
    // 這裡簡單實作：找「最前面」的敵人 (依路徑索引或距離終點)
    // 或找「第一個進入範圍」的
    for (const auto& enemy : m_enemies) {
        if (!enemy->isActive()) continue;
        if (Utils::distance(getPosition(), enemy->getPosition()) <= range) {
            return enemy;  // 簡單回傳第一個找到的
        }
    }
    return nullptr;
}