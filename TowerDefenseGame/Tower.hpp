#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include "Config.hpp"
#include "Enemy.hpp"
#include "Entity.hpp"
#include "Projectile.hpp"
#include "TowerData.hpp"
#include "Utils.hpp"

struct PlayerStats;

class Tower : public Entity {
public:
    Tower(sf::Vector2f pos,
        const std::vector<std::shared_ptr<Enemy>>& enemies,
        std::vector<std::unique_ptr<Projectile>>& projectiles,
        PlayerStats& stats);

    virtual ~Tower() = default;

    void update(sf::Time dt) override;
    void draw(sf::RenderWindow& window) override;

    sf::Vector2f getPosition() const override { return m_shape.getPosition(); }
    sf::FloatRect getBounds() const { return m_shape.getGlobalBounds(); }

    void upgrade();
    int getUpgradeCost() const;
    int getNextLevelDamage() const;
    bool isMaxLevel() const { return m_level >= 5; }

    float getEffectiveCooldown() const;

    std::string getName() const { return m_name; }
    TowerType getType() const { return m_type; }
    int getDamage() const { return m_damage; }
    int getLevel() const { return m_level; }
    int getPrice() const { return m_price; }

    using TextCallback = std::function<void(std::string, sf::Vector2f, sf::Color)>;
    void setTextCallback(TextCallback cb) { m_onTextRequest = cb; }

protected:
    virtual void performAction() = 0;
    std::shared_ptr<Enemy> findTarget(float range);

    TextCallback m_onTextRequest;

    sf::RectangleShape m_shape;

    const std::vector<std::shared_ptr<Enemy>>& m_enemies;
    std::vector<std::unique_ptr<Projectile>>& m_projectiles;
    PlayerStats& m_stats;

    std::string m_name;
    TowerType m_type;
    float m_range = 150.f;
    float m_cooldownTime = 1.0f;
    float m_currentCooldown = 0.f;
    int m_damage = 10;
    int m_price = 50;
    int m_level = 1;
};