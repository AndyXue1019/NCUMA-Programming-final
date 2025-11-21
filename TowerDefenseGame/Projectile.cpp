#include "Projectile.hpp"

#include "Enemy.hpp"
#include "Utils.hpp"

Projectile::Projectile(sf::Vector2f startPos, std::weak_ptr<Enemy> target, int damage)
    : m_target(target), m_damage(damage) {
    m_shape.setRadius(5.f);
    m_shape.setFillColor(sf::Color::Yellow);
    m_shape.setOrigin(sf::Vector2f(5.f, 5.f));
    m_shape.setPosition(startPos);
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
        destroy();
        return;
    }

    sf::Vector2f moveVec = Utils::normalize(direction) * m_speed * dt.asSeconds();
    m_shape.move(moveVec);
}

void Projectile::draw(sf::RenderWindow& window) {
    window.draw(m_shape);
}