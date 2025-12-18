#include "Tower.hpp"

#include "Accessory.hpp"
#include "PlayerStats.hpp"
#include "Utils.hpp"

Tower::Tower(sf::Vector2f pos,
             const std::vector<std::shared_ptr<Enemy>>& enemies,
             std::vector<std::unique_ptr<Projectile>>& projectiles,
             PlayerStats& stats)
    : m_enemies(enemies),
      m_projectiles(projectiles),
      m_stats(stats) {
    m_shape.setSize({40.f, 40.f});
    m_shape.setOrigin({20.f, 20.f});
    m_shape.setPosition(pos);
}

void Tower::update(sf::Time dt) {
    if (m_currentCooldown > 0.f) {
        m_currentCooldown -= dt.asSeconds();
    }
    if (m_currentCooldown <= 0.f) {
        performAction();
    }
}

float Tower::getEffectiveCooldown() const {
    float baseCooldown = m_cooldownTime;

    if (m_stats.isAccessoryActive(AccessoryType::StormGem)) {
        return baseCooldown / 1.5f;
    }
    return baseCooldown;
}

void Tower::draw(sf::RenderWindow& window) {
    window.draw(m_shape);
}

void Tower::upgrade() {
    if (isMaxLevel()) return;
    m_level++;
    m_damage = static_cast<int>(m_damage * 1.5f);
}

int Tower::getUpgradeCost() const {
    return m_price * m_level;
}

int Tower::getNextLevelDamage() const {
    return static_cast<int>(m_damage * 1.5f);
}

std::shared_ptr<Enemy> Tower::findTarget(float range) {
    std::shared_ptr<Enemy> bestCandidate = nullptr;

    for (const auto& enemy : m_enemies) {
        if (!enemy->isActive()) continue;

        // 檢查是否在攻擊範圍內
        float distToTower = Utils::distance(getPosition(), enemy->getPosition());
        if (distToTower > range) continue;

        if (bestCandidate == nullptr) {
            bestCandidate = enemy;
            continue;
        }

        // 比較 enemy 與 bestCandidate 誰離終點比較近
        // 判斷 A: 誰的 waypoint index 比較大 (代表走過的路段較多)
        std::size_t enemyIdx = enemy->getWaypointIndex();
        std::size_t bestIdx = bestCandidate->getWaypointIndex();

        if (enemyIdx > bestIdx) {
            bestCandidate = enemy;
        } else if (enemyIdx == bestIdx) {
            // 判斷 B: 如果 index 一樣 (在同一路段)，比較誰離下一個目標點更近
            // (離目標點越近 = 離終點越近)
            sf::Vector2f nextNode = enemy->getNextWaypointPos();  // 兩者目標點相同

            float distEnemy = Utils::distance(enemy->getPosition(), nextNode);
            float distBest = Utils::distance(bestCandidate->getPosition(), nextNode);

            if (distEnemy < distBest) {
                bestCandidate = enemy;
            }
        }
    }
    return bestCandidate;
}