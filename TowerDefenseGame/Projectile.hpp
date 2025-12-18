#pragma once
#include <memory>
#include <vector>

#include "Entity.hpp"

class Enemy;

class Projectile : public Entity {
   public:
    Projectile(sf::Vector2f startPos, std::weak_ptr<Enemy> target,
               const std::vector<std::shared_ptr<Enemy>>& enemies,
               int damage, bool isFire, bool isIce, bool isExplosive);

    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::Vector2f getPosition() const override { return m_shape.getPosition(); }

   private:
    sf::CircleShape m_shape;
    std::weak_ptr<Enemy> m_target;

    const std::vector<std::shared_ptr<Enemy>>& m_enemies;

    float m_speed = 400.f;
    int m_damage;
    bool m_isFire = false;
    bool m_isIce = false;
    bool m_isExplosive = false;

    // for explosion effect
    bool m_isExploding = false;
    sf::CircleShape m_shockwave;
    float m_explosionAlpha = 255.f;
    void updateExplosion(sf::Time dt);
};