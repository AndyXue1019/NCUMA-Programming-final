#pragma once
#include <SFML/Graphics.hpp>
#include "Tower.hpp"
#include "TowerData.hpp"
#include "Utils.hpp"
#include "PlayerStats.hpp" 
#include <cstdint>
#include <iostream>

// 1. Basic Tower
class BasicTower : public Tower {
public:
    // 移除 texture 參數
    BasicTower(sf::Vector2f pos, auto& e, auto& p, const PlayerStats& s)
        : Tower(pos, e, p, s)
    {
        const auto& info = TowerData::INFO.at(TowerType::Basic);
        m_name = info.name;
        m_price = info.price;
        // 改回設定顏色
        m_shape.setFillColor(info.color);

        m_range = 150.f;
        m_damage = 20;
        m_cooldownTime = 1.0f;
    }

protected:
    void performAction() override {
        auto target = findTarget(m_range);
        if (target) {
            bool isFire = false;
            if (m_stats.isAccessoryActive(AccessoryType::FireGem)) {
                int chance = 10 + 5 * m_level;
                if (rand() % 100 < chance) isFire = true;
            }
            m_projectiles.push_back(std::make_unique<Projectile>(getPosition(), target, m_damage, isFire));
            m_currentCooldown = m_cooldownTime;
        }
    }
};

// 2. Laser Tower
class LaserTower : public Tower {
public:
    LaserTower(sf::Vector2f pos, auto& e, auto& p, const PlayerStats& s)
        : Tower(pos, e, p, s)
    {
        const auto& info = TowerData::INFO.at(TowerType::Laser);
        m_name = info.name;
        m_price = info.price;
        m_shape.setFillColor(info.color);

        m_range = 120.f;
        m_damage = 10;
        m_cooldownTime = 0.5f;
    }

    void draw(sf::RenderWindow& window) override {
        Tower::draw(window);
        if (m_laserTimer > 0.f) {
            window.draw(m_laserBeam.data(), m_laserBeam.size(), sf::PrimitiveType::Lines);
        }
    }

    void update(sf::Time dt) override {
        Tower::update(dt);
        if (m_laserTimer > 0.f) m_laserTimer -= dt.asSeconds();
    }

protected:
    float m_laserTimer = 0.f;
    std::vector<sf::Vertex> m_laserBeam;

    void performAction() override {
        auto target = findTarget(m_range);
        if (target) {
            target->takeDamage(m_damage);
            m_currentCooldown = m_cooldownTime;

            bool isFire = false;
            if (m_stats.isAccessoryActive(AccessoryType::FireGem)) {
                int chance = 10 + 5 * m_level;
                if (rand() % 100 < chance) {
                    target->applyBurn(5.0f);
                    isFire = true;
                }
            }

            m_laserTimer = 0.05f;
            m_laserBeam.clear();
            sf::Color startColor = isFire ? sf::Color(255, 69, 0) : sf::Color::Red;
            sf::Color endColor = isFire ? sf::Color::Red : sf::Color::Yellow;

            m_laserBeam.push_back(sf::Vertex{ getPosition(), startColor });
            m_laserBeam.push_back(sf::Vertex{ target->getPosition(), endColor });
        }
    }
};

// 3. Sniper Tower
class SniperTower : public Tower {
public:
    SniperTower(sf::Vector2f pos, auto& e, auto& p, const PlayerStats& s)
        : Tower(pos, e, p, s) {
        const auto& info = TowerData::INFO.at(TowerType::Sniper);
        m_name = info.name;
        m_price = info.price;
        m_shape.setFillColor(info.color);

        m_range = 400.f;
        m_damage = 100;
        m_cooldownTime = 3.0f;
    }
protected:
    void performAction() override {
        auto target = findTarget(m_range);
        if (target) {
            bool isFire = false;
            if (m_stats.isAccessoryActive(AccessoryType::FireGem)) {
                int chance = 10 + 5 * m_level;
                if (rand() % 100 < chance) isFire = true;
            }
            m_projectiles.push_back(std::make_unique<Projectile>(getPosition(), target, m_damage, isFire));
            m_currentCooldown = m_cooldownTime;
        }
    }
};

// 4. Slow Tower
class SlowTower : public Tower {
public:
    SlowTower(sf::Vector2f pos, auto& e, auto& p, const PlayerStats& s)
        : Tower(pos, e, p, s) {
        const auto& info = TowerData::INFO.at(TowerType::Slow);
        m_name = info.name;
        m_price = info.price;
        m_shape.setFillColor(info.color);

        m_range = 100.f;
        m_cooldownTime = 0.5f;
    }
protected:
    void performAction() override {
        bool hitAny = false;
        for (const auto& enemy : m_enemies) {
            if (enemy->isActive() && Utils::distance(getPosition(), enemy->getPosition()) <= m_range) {
                enemy->applySlow(0.5f, 1.0f);
                hitAny = true;

                if (m_stats.isAccessoryActive(AccessoryType::FireGem)) {
                    if (rand() % 100 < (10 + 5 * m_level)) enemy->applyBurn(5.0f);
                }
            }
        }
        if (hitAny) m_currentCooldown = m_cooldownTime;
    }
};

// 5. Teleport Tower
class TeleportTower : public Tower {
public:
    TeleportTower(sf::Vector2f pos, auto& e, auto& p, const PlayerStats& s)
        : Tower(pos, e, p, s) {
        const auto& info = TowerData::INFO.at(TowerType::Teleport);
        m_name = info.name;
        m_price = info.price;
        m_shape.setFillColor(info.color);

        m_range = 200.f;
        m_cooldownTime = 5.0f;
    }
protected:
    void performAction() override {
        auto target = findTarget(m_range);
        if (target) {
            target->teleportBack(3);
            m_currentCooldown = m_cooldownTime;
        }
    }
};

// 6. Self Destruct Tower
class SelfDestructTower : public Tower {
public:
    SelfDestructTower(sf::Vector2f pos, auto& e, auto& p, const PlayerStats& s)
        : Tower(pos, e, p, s) {
        const auto& info = TowerData::INFO.at(TowerType::SelfDestruct);
        m_name = info.name;
        m_price = info.price;
        m_shape.setFillColor(info.color);

        m_range = 80.f;
        m_damage = 500;
        m_cooldownTime = 0.1f;

        m_shockwave.setRadius(10.f);
        m_shockwave.setOrigin({ 10.f, 10.f });
        m_shockwave.setPosition(pos);
        m_shockwave.setFillColor(sf::Color::Transparent);
        m_shockwave.setOutlineColor(sf::Color(255, 100, 0, 255));
        m_shockwave.setOutlineThickness(5.f);
    }

    void update(sf::Time dt) override {
        if (m_isExploding) {
            updateExplosion(dt);
            return;
        }
        Tower::update(dt);
    }

    void draw(sf::RenderWindow& window) override {
        if (!m_isExploding) Tower::draw(window);
        else window.draw(m_shockwave);
    }

protected:
    bool m_isExploding = false;
    sf::CircleShape m_shockwave;
    float m_explosionRadius = 150.f;
    float m_explosionAlpha = 255.f;

    void performAction() override {
        bool triggered = false;
        for (const auto& enemy : m_enemies) {
            if (enemy->isActive() && Utils::distance(getPosition(), enemy->getPosition()) <= m_range) {
                triggered = true;
                break;
            }
        }
        if (triggered) {
            for (const auto& target : m_enemies) {
                if (target->isActive() && Utils::distance(getPosition(), target->getPosition()) <= m_explosionRadius) {
                    target->takeDamage(m_damage);
                }
            }
            m_isExploding = true;
        }
    }

    void updateExplosion(sf::Time dt) {
        float currentRadius = m_shockwave.getRadius();
        float expansionSpeed = 300.f;
        float newRadius = currentRadius + expansionSpeed * dt.asSeconds();

        m_shockwave.setRadius(newRadius);
        m_shockwave.setOrigin({ newRadius, newRadius });

        float fadeSpeed = 400.f;
        m_explosionAlpha -= fadeSpeed * dt.asSeconds();
        if (m_explosionAlpha <= 0.f) {
            m_explosionAlpha = 0.f;
            this->destroy();
        }
        sf::Color c = m_shockwave.getOutlineColor();
        c.a = static_cast<std::uint8_t>(m_explosionAlpha);
        m_shockwave.setOutlineColor(c);
    }
};