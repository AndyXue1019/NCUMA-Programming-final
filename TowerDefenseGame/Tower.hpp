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

struct PlayerStats;

class Tower : public Entity {
public:
    // 修改建構子：移除 Texture 參數
    Tower(sf::Vector2f pos,
        const std::vector<std::shared_ptr<Enemy>>& enemies,
        std::vector<std::unique_ptr<Projectile>>& projectiles,
        const PlayerStats& stats);

    virtual ~Tower() = default;

    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;

    // 改回使用 m_shape
    sf::Vector2f getPosition() const override { return m_shape.getPosition(); }
    sf::FloatRect getBounds() const { return m_shape.getGlobalBounds(); }

    void upgrade();
    int getUpgradeCost() const;
    int getNextLevelDamage() const;
    bool isMaxLevel() const { return m_level >= 5; }

    std::string getName() const { return m_name; }
    int getDamage() const { return m_damage; }
    int getLevel() const { return m_level; }
    int getPrice() const { return m_price; }

protected:
    virtual void performAction() = 0;
    std::shared_ptr<Enemy> findTarget(float range);

    // 改回 RectangleShape
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