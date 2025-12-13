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
        std::vector<std::unique_ptr<Projectile>>& p, const PlayerStats& s)
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
        std::vector<std::unique_ptr<Projectile>>& p, const PlayerStats& s)
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
        std::vector<std::unique_ptr<Projectile>>& p, const PlayerStats& s)
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
        std::vector<std::unique_ptr<Projectile>>& p, const PlayerStats& s)
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
        std::vector<std::unique_ptr<Projectile>>& p, const PlayerStats& s)
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
        std::vector<std::unique_ptr<Projectile>>& p, const PlayerStats& s)
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
        std::vector<std::unique_ptr<Projectile>>& p, PlayerStats& s) // 注意: 這裡要傳 PlayerStats& 而非 const，因為要加錢
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
            // [新增] 每次攻擊獲得 5 金幣 (addGold 內已包含 WealthDiamond 判斷)
            // 這裡需要 const_cast 或是把 m_stats 改為 non-const reference
            // 為了方便，我們在 Game.cpp 傳遞時已經將 PlayerStats 改為 reference
            // 但 Tower 基礎類別存的是 const PlayerStats&，這裡我們偷用 const_cast
            const_cast<PlayerStats&>(m_stats).addGold(5);

            // TODO: 在這裡彈出 "+$5" 或 "+$10" 的漂浮文字
            // 由於 Tower 類別無法直接存取 Game 的 UI 層，這裡暫時省略，
            // 實務上可以傳入一個 callback function 來生成文字。

            // 骰機率
            int r = Utils::m_rand(); // 0-99

            int finalDmg = getDamage();
            bool isInstantKill = false;
            bool isDouble = false;
            bool isZero = false;

            if (r < 5) {
                // 5% 秒殺
                isInstantKill = true;
            }
            else if (r < 25) {
                // 20% 雙倍 (5~24)
                finalDmg *= 2;
                isDouble = true;
            }
            else if (r < 50) {
                // 25% 變為 0 (25~49)
                finalDmg = 0;
                isZero = true;
            }
            // 剩下 50% 正常傷害

            // 發射子彈，但這邊直接結算傷害會比較容易做特效
            // 或者我們可以修改 Projectile 讓它攜帶特殊 flag
            // 這裡簡單起見，我們直接對目標造成傷害 (類似 Laser) 
            // 但為了要有彈道，我們還是生成 Projectile，但要在 Projectile 增加屬性

            // 這裡我們用一個變通方法：生成 Projectile，把計算好的 damage 傳進去
            // 若是秒殺，傳一個巨大的數字

            if (isInstantKill) finalDmg = 999999;

            // 寶石效果依然有效
            bool isFire = m_stats.isAccessoryActive(AccessoryType::FireGem) && (Utils::m_rand() < 50);
            bool isIce = m_stats.isAccessoryActive(AccessoryType::IceGem) && (Utils::m_rand() < 20);
            bool isExplosive = m_stats.isAccessoryActive(AccessoryType::ExplosiveGem) && (Utils::m_rand() < 20);

            // 為了視覺效果，如果秒殺或雙倍，我們可以傳入特殊的 Projectile 顏色
            // 這裡先用標準生成
            m_projectiles.push_back(std::make_unique<Projectile>(
                getPosition(), target, m_enemies, finalDmg, isFire, isIce, isExplosive
            ));

            m_currentCooldown = getEffectiveCooldown();
        }
    }

private:
    int m_baseDamage;
};