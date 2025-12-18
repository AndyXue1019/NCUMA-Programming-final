#include "Projectile.hpp"

#include <iostream>  // for debug

#include "Enemy.hpp"
#include "Utils.hpp"

Projectile::Projectile(sf::Vector2f startPos, std::weak_ptr<Enemy> target,
                       const std::vector<std::shared_ptr<Enemy>>& enemies,
                       int damage, bool isFire, bool isIce, bool isExplosive)
    : m_target(target), m_enemies(enemies), m_damage(damage), m_isFire(isFire), m_isIce(isIce), m_isExplosive(isExplosive) {
    m_shape.setRadius(5.f);
    m_shape.setOrigin({5.f, 5.f});
    m_shape.setPosition(startPos);

    // 根據效果改變顏色
    if (m_isExplosive)
        m_shape.setFillColor(sf::Color(139, 0, 0));  // 深紅
    else if (m_isIce)
        m_shape.setFillColor(sf::Color::Cyan);  // 青色
    else if (m_isFire)
        m_shape.setFillColor(sf::Color(255, 69, 0));  // 橘紅
    else
        m_shape.setFillColor(sf::Color::Yellow);
}

void Projectile::update(sf::Time dt) {
    auto targetSp = m_target.lock();
    if (!targetSp || !targetSp->isActive()) {
        destroy();
        return;
    }

    sf::Vector2f targetPos = targetSp->getPosition();
    sf::Vector2f direction = targetPos - m_shape.getPosition();

    if (Utils::distance(m_shape.getPosition(), targetPos) < 20.f) {
        // --- 命中處理 ---

        // 1. 基礎傷害
        targetSp->takeDamage(m_damage);

        // 2. 火焰效果
        if (m_isFire) targetSp->applyBurn(5.0f);

        // 3. 寒冰效果 (暫停 1 秒)
        if (m_isIce) targetSp->applyStun(1.0f);

        // 4. 爆裂效果 (小範圍 AOE)
        if (m_isExplosive) {
            float range = 100.f;              // 爆炸半徑
            int explosionDmg = m_damage * 2;  // 200% 傷害

            for (const auto& enemy : m_enemies) {
                if (enemy->isActive() && Utils::distance(m_shape.getPosition(), enemy->getPosition()) <= range) {
                    // 對範圍內敵人造成傷害 (包含目標自己，可能會受到兩次傷害，視設計而定)
                    // 這裡簡單做：對範圍內「所有」敵人造成爆炸傷
                    // 如果不想讓主目標受兩次傷，可以判斷 if (enemy != targetSp)
                    enemy->takeDamage(explosionDmg);
                }
            }
            // 可以加個特效 (這裡略)
        }

        destroy();
        return;
    }

    sf::Vector2f moveVec = Utils::normalize(direction) * m_speed * dt.asSeconds();
    m_shape.move(moveVec);
}

void Projectile::draw(sf::RenderWindow& window) {
    window.draw(m_shape);
}