#include "Projectile.hpp"

#include "Enemy.hpp"
#include "Utils.hpp"

Projectile::Projectile(sf::Vector2f startPos, std::weak_ptr<Enemy> target, int damage, bool isFire)
    : m_target(target), m_damage(damage) {
    m_shape.setRadius(5.f);
    m_shape.setFillColor(sf::Color::Yellow);
    m_shape.setOrigin(sf::Vector2f(5.f, 5.f));
    m_shape.setPosition(startPos);

    if (m_isFire) {
        m_shape.setFillColor(sf::Color(255, 69, 0)); // 火焰彈變橘紅色
    }
    else {
        m_shape.setFillColor(sf::Color::Yellow);
    }
}

void Projectile::update(sf::Time dt) {
    auto targetSp = m_target.lock();

    // 如果 targetSp 為空 (敵人已被刪除) 或者 不活躍
    if (!targetSp || !targetSp->isActive()) {
        destroy();
        return;
    }

    sf::Vector2f targetPos = targetSp->getPosition();
    sf::Vector2f direction = targetPos - m_shape.getPosition();

    if (Utils::distance(m_shape.getPosition(), targetPos) < 20.f) {
        targetSp->takeDamage(m_damage);

        if (m_isFire) {
            targetSp->applyBurn(5.0f); // 持續 5 秒
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