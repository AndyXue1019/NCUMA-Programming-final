#pragma once
#include <SFML/Graphics.hpp>
#include <cstdint>
#include <iostream>
#include <format>
#include "PlayerStats.hpp"
#include "Tower.hpp"
#include "TowerData.hpp"
#include "Utils.hpp"

// 1. Basic Tower: 基礎塔
class BasicTower : public Tower {
public:
    BasicTower(sf::Vector2f pos, const std::vector<std::shared_ptr<Enemy>>& e,
        std::vector<std::unique_ptr<Projectile>>& p, PlayerStats& s)
        : Tower(pos, e, p, s)
    {
        m_type = TowerType::Basic;
        const auto& info = TowerData::INFO.at(m_type);
        m_name = info.name;
        m_price = info.price;
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
            bool isIce = false;
            bool isExplosive = false;

            // --- 1. 火焰寶石 (機率: 10 + 5 * Level %) ---
            if (m_stats.isAccessoryActive(AccessoryType::FireGem)) {
                int chance = 10 + 5 * m_level;
                if (Utils::m_rand() < chance) isFire = true;
            }

            // --- 2. 寒冰寶石 (機率: 3%) ---
            if (m_stats.isAccessoryActive(AccessoryType::IceGem)) {
				int chance = 3 + 2 * m_level;
                if (Utils::m_rand() < chance) isIce = true;
            }

            // --- 3. 爆裂寶石 (機率: 10%) ---
            if (m_stats.isAccessoryActive(AccessoryType::ExplosiveGem)) {
				int chance = 10 + 3 * m_level;
                if (Utils::m_rand() < chance) isExplosive = true;
            }

            // 發射子彈 (傳入所有效果 flag)
            m_projectiles.push_back(std::make_unique<Projectile>(
                getPosition(), target, m_enemies, m_damage, isFire, isIce, isExplosive
            ));

            // 使用 getEffectiveCooldown() 支援風暴寶石加速
            m_currentCooldown = getEffectiveCooldown();
        }
    }
};

// 2. Laser Tower: 雷射塔 (直接傷害，無子彈)
class LaserTower : public Tower {
public:
    LaserTower(sf::Vector2f pos, const std::vector<std::shared_ptr<Enemy>>& e,
        std::vector<std::unique_ptr<Projectile>>& p, PlayerStats& s)
        : Tower(pos, e, p, s)
    {
        m_type = TowerType::Laser;
        const auto& info = TowerData::INFO.at(m_type);
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
            // 基礎傷害
            target->takeDamage(m_damage);
            m_currentCooldown = getEffectiveCooldown(); // 支援風暴寶石

            bool triggeredEffect = false;

            // --- 寶石效果判定 (直接對敵人作用) ---

            // 1. 火焰
            if (m_stats.isAccessoryActive(AccessoryType::FireGem) && Utils::m_rand() < (10 + 5 * m_level)) {
                target->applyBurn(5.0f);
                triggeredEffect = true;
            }
            // 2. 寒冰
            if (m_stats.isAccessoryActive(AccessoryType::IceGem) && Utils::m_rand() < 3) {
                target->applyStun(1.0f);
                triggeredEffect = true;
            }
            // 3. 爆裂 (雷射觸發爆炸)
            if (m_stats.isAccessoryActive(AccessoryType::ExplosiveGem) && Utils::m_rand() < 10) {
                float range = 100.f;
                int explosionDmg = m_damage * 2;
                for (const auto& enemy : m_enemies) {
                    if (enemy->isActive() && Utils::distance(target->getPosition(), enemy->getPosition()) <= range) {
                        enemy->takeDamage(explosionDmg);
                    }
                }
                triggeredEffect = true;
            }

            // 雷射視覺效果
            m_laserTimer = 0.05f;
            m_laserBeam.clear();

            // 如果觸發特效，雷射變橘色；否則為紅色
            sf::Color startColor = triggeredEffect ? sf::Color(255, 69, 0) : sf::Color::Red;
            sf::Color endColor = triggeredEffect ? sf::Color::Red : sf::Color::Yellow;

            m_laserBeam.push_back(sf::Vertex{ getPosition(), startColor });
            m_laserBeam.push_back(sf::Vertex{ target->getPosition(), endColor });
        }
    }
};

// 3. Sniper Tower: 狙擊塔
class SniperTower : public Tower {
public:
    SniperTower(sf::Vector2f pos, const std::vector<std::shared_ptr<Enemy>>& e,
        std::vector<std::unique_ptr<Projectile>>& p, PlayerStats& s)
        : Tower(pos, e, p, s)
    {
        m_type = TowerType::Sniper;
        const auto& info = TowerData::INFO.at(m_type);
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
            bool isIce = false;
            bool isExplosive = false;

            if (m_stats.isAccessoryActive(AccessoryType::FireGem)) {
                if (Utils::m_rand() < (10 + 5 * m_level)) isFire = true;
            }
            if (m_stats.isAccessoryActive(AccessoryType::IceGem)) {
                if (Utils::m_rand() < 3) isIce = true;
            }
            if (m_stats.isAccessoryActive(AccessoryType::ExplosiveGem)) {
                if (Utils::m_rand() < 10) isExplosive = true;
            }

            m_projectiles.push_back(std::make_unique<Projectile>(
                getPosition(), target, m_enemies, m_damage, isFire, isIce, isExplosive
            ));

            m_currentCooldown = getEffectiveCooldown(); // 支援風暴寶石
        }
    }
};

// 4. Slow Tower: 緩速塔
class SlowTower : public Tower {
public:
    SlowTower(sf::Vector2f pos, const std::vector<std::shared_ptr<Enemy>>& e,
        std::vector<std::unique_ptr<Projectile>>& p, PlayerStats& s)
        : Tower(pos, e, p, s)
    {
        m_type = TowerType::Slow;
        const auto& info = TowerData::INFO.at(m_type);
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
                // 基礎效果：緩速
                enemy->applySlow(0.5f, 1.0f);
                hitAny = true;

                // 緩速塔也能觸發寶石效果 (AOE 觸發)
                // 1. 火焰
                if (m_stats.isAccessoryActive(AccessoryType::FireGem)) {
                    if (Utils::m_rand() < (10 + 5 * m_level)) enemy->applyBurn(5.0f);
                }
                // 2. 寒冰 (雙重控制！)
                if (m_stats.isAccessoryActive(AccessoryType::IceGem)) {
                    if (Utils::m_rand() < 3) enemy->applyStun(1.0f);
                }
                // 3. 爆裂
                if (m_stats.isAccessoryActive(AccessoryType::ExplosiveGem)) {
                    if (Utils::m_rand() < 10) enemy->takeDamage(50); // 緩速塔本身沒傷害，給予固定傷害
                }
            }
        }
        if (hitAny) m_currentCooldown = getEffectiveCooldown(); // 支援風暴寶石
    }
};

// 5. Teleport Tower: 傳送塔
class TeleportTower : public Tower {
public:
    TeleportTower(sf::Vector2f pos, const std::vector<std::shared_ptr<Enemy>>& e,
        std::vector<std::unique_ptr<Projectile>>& p, PlayerStats& s)
        : Tower(pos, e, p, s)
    {
        m_type = TowerType::Teleport;
        const auto& info = TowerData::INFO.at(m_type);
        m_name = info.name;
        m_price = info.price;
        m_shape.setFillColor(info.color);

        m_range = 200.f;
        m_cooldownTime = 5.0f;
    }
protected:
    void performAction() override {
        // 根據等級傳送多個目標回去
        for (int i = 0; i < m_level; ++i) {
           auto target = findTarget(m_range);
               if (target) {
                   target->teleportBack(3);
                   m_currentCooldown = getEffectiveCooldown(); // 支援風暴寶石
               }
         }
    }
};

// 6. Self Destruct Tower: 自爆塔
class SelfDestructTower : public Tower {
public:
    SelfDestructTower(sf::Vector2f pos, const std::vector<std::shared_ptr<Enemy>>& e,
        std::vector<std::unique_ptr<Projectile>>& p, PlayerStats& s)
        : Tower(pos, e, p, s)
    {
        m_type = TowerType::SelfDestruct;
        const auto& info = TowerData::INFO.at(m_type);
        m_name = info.name;
        m_price = info.price;
        m_shape.setFillColor(info.color);

        m_range = 80.f;
        m_damage = 100;
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
            // 自爆傷害
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

// 7. Gambler Tower: 終極賭神塔
class GamblerTower : public Tower {
public:
    GamblerTower(sf::Vector2f pos, const std::vector<std::shared_ptr<Enemy>>& e,
        std::vector<std::unique_ptr<Projectile>>& p, PlayerStats& s)
        : Tower(pos, e, p, s)
    {
        m_type = TowerType::Gambler;
        const auto& info = TowerData::INFO.at(m_type);
        m_name = info.name;
        m_price = 0;
        m_shape.setFillColor(info.color);
        m_shape.setOutlineColor(sf::Color::White);
        m_shape.setOutlineThickness(3.f);

        m_range = 300.f;
        m_cooldownTime = 0.2f; // 射速很快

        // 初始值為一般塔的 3 倍 (Basic Dmg=20) -> 60
        m_baseDamage = 60;
    }

    // 覆寫 getDamage，根據金錢計算
    int getDamage() const {
        // 每擁有 20 金幣基礎數值 +2
        int bonus = (m_stats.gold / 20) * 2;
        return m_baseDamage + bonus;
    }

    // 覆寫 update 來顯示特殊特效或光環
    void update(sf::Time dt) override {
        Tower::update(dt);

        // --- 90% 緩速光環 ---
        for (const auto& enemy : m_enemies) {
            if (enemy->isActive() && Utils::distance(getPosition(), enemy->getPosition()) <= m_range) {
                // 90% 減速 = 速度剩 0.1 倍
                enemy->applySlow(0.1f, 0.1f); // 持續時間短，因為每幀更新
            }
        }
    }

    // 覆寫 performAction 處理特殊的攻擊機率
    void performAction() override {
        auto target = findTarget(m_range);
        if (target) {

            m_stats.addGold(5);
            if (m_onTextRequest) {
                m_onTextRequest("+$5", getPosition() + sf::Vector2f(0, -30), sf::Color::Yellow);
            }

            // 骰機率
            int r = Utils::m_rand(); // 0-99

            int finalDmg = getDamage();
            if (r < 5) {
                // 5% 秒殺
                finalDmg = 999999;
                if (m_onTextRequest) {
                    m_onTextRequest("JACKPOT!", target->getPosition(), sf::Color::Red);
                }
            }
            else if (r < 25) {
                // 20% 雙倍
                finalDmg *= 2;
                if (m_onTextRequest) {
                    m_onTextRequest("DOUBLE!", target->getPosition(), sf::Color(255, 140, 0)); // 深橘色
                }
            }
            else if (r < 50) {
                // 25% 失敗 (0傷)
                finalDmg = 0;
                if (m_onTextRequest) {
                    m_onTextRequest("MISS...", target->getPosition(), sf::Color(150, 150, 150)); // 灰色
                }
            }

            bool isFire = m_stats.isAccessoryActive(AccessoryType::FireGem) && (Utils::m_rand() < 50);
            bool isIce = m_stats.isAccessoryActive(AccessoryType::IceGem) && (Utils::m_rand() < 20);
            bool isExplosive = m_stats.isAccessoryActive(AccessoryType::ExplosiveGem) && (Utils::m_rand() < 20);

            
            m_projectiles.push_back(std::make_unique<Projectile>(
                getPosition(), target, m_enemies, finalDmg, isFire, isIce, isExplosive
            ));

            m_currentCooldown = getEffectiveCooldown();
        }
    }

private:
    int m_baseDamage;
};