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
    if (m_isExplosive) {
        m_shape.setFillColor(sf::Color(139, 0, 0));  // 深紅
        // setup shockwave only for explosive projectiles
        m_shockwave.setRadius(10.f);
        m_shockwave.setOrigin({10.f, 10.f});
        m_shockwave.setFillColor(sf::Color::Transparent);     // 內部透明
        m_shockwave.setOutlineColor(sf::Color(255, 100, 0));  // 橘紅色邊框
        m_shockwave.setOutlineThickness(3.f);
    } else if (m_isIce)
        m_shape.setFillColor(sf::Color::Cyan);  // 青色
    else if (m_isFire)
        m_shape.setFillColor(sf::Color(255, 69, 0));  // 橘紅
    else
        m_shape.setFillColor(sf::Color::Yellow);
}

void Projectile::update(sf::Time dt) {
    if (m_isExploding) {
        updateExplosion(dt);
        return;
    }

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
            float range = 100.f;                                  // 爆炸半徑
            int explosionDmg = static_cast<int>(m_damage * 0.8);  // 80% 傷害

            for (const auto& enemy : m_enemies) {
                if (enemy->isActive() && Utils::distance(m_shape.getPosition(), enemy->getPosition()) <= range) {
                    enemy->takeDamage(explosionDmg);
                }
            }

            m_isExploding = true;
            m_shockwave.setPosition(m_shape.getPosition());

            return;  // 等待下一次 update 處理爆炸動畫
        }

        destroy();
        return;
    }

    sf::Vector2f moveVec = Utils::normalize(direction) * m_speed * dt.asSeconds();
    m_shape.move(moveVec);
}

void Projectile::draw(sf::RenderWindow& window) {
    if (m_isExploding) {
        window.draw(m_shockwave);
    } else {
        window.draw(m_shape);
    }
}

void Projectile::updateExplosion(sf::Time dt) {
    float currentRadius = m_shockwave.getRadius();
    float expansionSpeed = 300.f;  // 擴散速度
    float newRadius = currentRadius + expansionSpeed * dt.asSeconds();

    m_shockwave.setRadius(newRadius);
    m_shockwave.setOrigin({newRadius, newRadius});  // 保持中心對齊

    float fadeSpeed = 500.f;  // 淡出速度 (比自爆塔快一點)
    m_explosionAlpha -= fadeSpeed * dt.asSeconds();

    // 當完全透明時，才真正銷毀物件
    if (m_explosionAlpha <= 0.f) {
        m_explosionAlpha = 0.f;
        this->destroy();
    }

    sf::Color c = m_shockwave.getOutlineColor();
    c.a = static_cast<std::uint8_t>(m_explosionAlpha);
    m_shockwave.setOutlineColor(c);
}