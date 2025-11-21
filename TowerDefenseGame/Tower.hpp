#pragma once
#include "Entity.hpp"
#include "Enemy.hpp"
#include "Projectile.hpp"
#include "Utils.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

enum class TowerType {
    Basic,
    Laser,
    Sniper,
    Slow,
    Teleport,
    SelfDestruct
};

class Tower : public Entity {
   public:
    // 這裡傳入 worldEnemies 和 worldProjectiles 是為了讓塔能偵測敵人和生成子彈
    Tower(sf::Vector2f pos, const std::vector<std::shared_ptr<Enemy>>& enemies,
          std::vector<std::unique_ptr<Projectile>>& projectiles);

    virtual ~Tower() = default;

    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::Vector2f getPosition() const override { return m_shape.getPosition(); }

    int getLevel() const { return m_level; }
    int getPrice() const { return m_price; }

   protected:
    // 讓子類別實作具體攻擊邏輯
    virtual void performAction() = 0;

    // 輔助函式：尋找範圍內最近/最遠/血量最高的敵人
    std::shared_ptr<Enemy> findTarget(float range);

    sf::RectangleShape m_shape;
    const std::vector<std::shared_ptr<Enemy>>& m_enemies;
    std::vector<std::unique_ptr<Projectile>>& m_projectiles;

    // 屬性
    std::string m_name;
    float m_range = 150.f;
    float m_cooldownTime = 1.0f;
    float m_currentCooldown = 0.f;
    int m_damage = 10;
    int m_price = 50;
    int m_level = 1;
};