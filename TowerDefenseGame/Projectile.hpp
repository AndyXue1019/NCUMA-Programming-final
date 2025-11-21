#pragma once
#include "Entity.hpp"

// 前向宣告，因為子彈需要知道 Enemy 類別
class Enemy;

class Projectile : public Entity {
   public:
    Projectile(sf::Vector2f startPos, std::weak_ptr<Enemy> target, int damage);

    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::Vector2f getPosition() const override { return m_shape.getPosition(); }

   private:
    sf::CircleShape m_shape;
    std::weak_ptr<Enemy> m_target;  // 追蹤目標
    float m_speed = 400.f;
    int m_damage;
};