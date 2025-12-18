#pragma once
#include <memory>
#include <vector>

#include "Entity.hpp"

class Enemy;

class Projectile : public Entity {
   public:
    // 修改建構子：
    // 1. 傳入所有敵人列表 (為了爆炸 AOE)
    // 2. 新增 isIce, isExplosive 參數
    Projectile(sf::Vector2f startPos, std::weak_ptr<Enemy> target,
               const std::vector<std::shared_ptr<Enemy>>& enemies,      // 新增
               int damage, bool isFire, bool isIce, bool isExplosive);  // 新增 flags

    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::Vector2f getPosition() const override { return m_shape.getPosition(); }

   private:
    sf::CircleShape m_shape;
    std::weak_ptr<Enemy> m_target;

    // 儲存敵人列表的參考 (注意：要確保 Projectile 存活期間敵人列表有效，通常在 Game 中這成立)
    const std::vector<std::shared_ptr<Enemy>>& m_enemies;

    float m_speed = 400.f;
    int m_damage;
    bool m_isFire = false;
    bool m_isIce = false;
    bool m_isExplosive = false;
};