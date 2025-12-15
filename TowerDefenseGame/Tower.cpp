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
    m_stats(stats)
{
    m_shape.setSize({ 40.f, 40.f });
    m_shape.setOrigin({ 20.f, 20.f });
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
    for (const auto& enemy : m_enemies) {
        if (!enemy->isActive()) continue;
        if (Utils::distance(getPosition(), enemy->getPosition()) <= range) {
            return enemy;
        }
    }
    return nullptr;
}