#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>

#include "Enemy.hpp"
#include "Entity.hpp"
#include "Projectile.hpp"
#include "Utils.hpp"
#include "Config.hpp" 

// 前向宣告
struct PlayerStats;

class Tower : public Entity {
public:
    Tower(sf::Vector2f pos,
        const std::vector<std::shared_ptr<Enemy>>& enemies,
        std::vector<std::unique_ptr<Projectile>>& projectiles,
        const PlayerStats& stats);

    virtual ~Tower() = default;

    // 只宣告，不實作
    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;

    sf::Vector2f getPosition() const override { return m_shape.getPosition(); }
    sf::FloatRect getBounds() const { return m_shape.getGlobalBounds(); }

    void upgrade();
    int getUpgradeCost() const;
    int getNextLevelDamage() const;
    bool isMaxLevel() const { return m_level >= 5; }

    // [修正] 新增這個函式的宣告
    float getEffectiveCooldown() const;

    std::string getName() const { return m_name; }
    int getDamage() const { return m_damage; }
    int getLevel() const { return m_level; }
    int getPrice() const { return m_price; }

protected:
    virtual void performAction() = 0;
    std::shared_ptr<Enemy> findTarget(float range);

    sf::RectangleShape m_shape;

    const std::vector<std::shared_ptr<Enemy>>& m_enemies;
    std::vector<std::unique_ptr<Projectile>>& m_projectiles;
    const PlayerStats& m_stats;

    std::string m_name;
    float m_range = 150.f;
    float m_cooldownTime = 1.0f;
    float m_currentCooldown = 0.f;
    int m_damage = 10;
    int m_price = 50;
    int m_level = 1;
};