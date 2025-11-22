#pragma once
#include <SFML/Graphics.hpp>
#include "Tower.hpp"
#include "TowerData.hpp"
#include "Utils.hpp"
#include <cstdint>
#include <iostream>

// Towers:
// 1. Basic Tower: 均衡型
class BasicTower : public Tower {
   public:
    BasicTower(sf::Vector2f pos, auto& e, auto& p) : Tower(pos, e, p) {
        const auto& info = TowerData::INFO.at(TowerType::Basic);
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
            // 發射普通子彈
            m_projectiles.push_back(std::make_unique<Projectile>(getPosition(), target, m_damage));
            m_currentCooldown = m_cooldownTime;
        }
    }
};

// 2. Laser Tower: 高攻速，低傷害
class LaserTower : public Tower {
   public:
    LaserTower(sf::Vector2f pos, auto& e, auto& p) : Tower(pos, e, p) {
        const auto& info = TowerData::INFO.at(TowerType::Laser);
        m_name = info.name;
        m_price = info.price;
        m_shape.setFillColor(info.color);
        
        m_range = 120.f;
        m_damage = 10;          // 低傷
        m_cooldownTime = 0.5f;  // 極快
    }

    void draw(sf::RenderWindow& window) override {
        Tower::draw(window);
        if (m_laserTimer > 0.f) {
            window.draw(m_laserBeam.data(), m_laserBeam.size(), sf::PrimitiveType::Lines);
        }
    }

    void update(sf::Time dt) override {
        Tower::update(dt);  // 冷卻

        if (m_laserTimer > 0.f) {
            m_laserTimer -= dt.asSeconds();
        }
    }

   protected:
    float m_laserTimer = 0.f;
    std::vector<sf::Vertex> m_laserBeam;

    void performAction() override {
        auto target = findTarget(m_range);
        if (target) {
            target->takeDamage(m_damage);
            m_currentCooldown = m_cooldownTime;

            // 雷射特效
            m_laserTimer = 0.05f;  // 雷射顯示 0.05 秒

            m_laserBeam.clear();
            // 起點：塔的中心
            m_laserBeam.push_back(sf::Vertex{getPosition(), sf::Color::Red});
            // 終點：敵人的中心
            m_laserBeam.push_back(sf::Vertex{target->getPosition(), sf::Color::Yellow});
        }
    }
};

// 3. Sniper Tower: 超遠程，高傷害，慢速
class SniperTower : public Tower {
   public:
    SniperTower(sf::Vector2f pos, auto& e, auto& p) : Tower(pos, e, p) {
        const auto& info = TowerData::INFO.at(TowerType::Sniper);
        m_name = info.name;
        m_price = info.price;
        m_shape.setFillColor(info.color);

        m_range = 400.f;        // 超遠
        m_damage = 100;         // 高傷
        m_cooldownTime = 3.0f;  // 慢
    }

   protected:
    void performAction() override {
        auto target = findTarget(m_range);
        if (target) {
            m_projectiles.push_back(std::make_unique<Projectile>(getPosition(), target, m_damage));
            m_currentCooldown = m_cooldownTime;
        }
    }
};

// 4. Slow Tower: 範圍緩速 (無子彈)
class SlowTower : public Tower {
   public:
    SlowTower(sf::Vector2f pos, auto& e, auto& p) : Tower(pos, e, p) {
        const auto& info = TowerData::INFO.at(TowerType::Slow);
        m_name = info.name;
        m_price = info.price;
        m_shape.setFillColor(info.color);  // Cyan

        m_range = 100.f;
        m_cooldownTime = 0.5f;  // 每 0.5 秒刷新一次緩速狀態
    }

   protected:
    void performAction() override {
        bool hitAny = false;
        // 掃描範圍內所有敵人
        for (const auto& enemy : m_enemies) {
            if (enemy->isActive() && Utils::distance(getPosition(), enemy->getPosition()) <= m_range) {
                // 緩速 50%，持續 1 秒
                enemy->applySlow(0.5f, 1.0f);
                hitAny = true;
            }
        }
        if (hitAny) m_currentCooldown = m_cooldownTime;
    }
};

// 5. Teleport Tower: 傳送敵人回起點
class TeleportTower : public Tower {
   public:
    TeleportTower(sf::Vector2f pos, auto& e, auto& p) : Tower(pos, e, p) {
        const auto& info = TowerData::INFO.at(TowerType::Teleport);
        m_name = info.name;
        m_price = info.price;
        m_shape.setFillColor(sf::Color::Magenta);

        m_range = 200.f;
        m_cooldownTime = 5.0f;  // 長冷卻
    }

   protected:
    void performAction() override {
        auto target = findTarget(m_range);
        if (target) {
            // 將敵人往回傳送 3 個節點
            target->teleportBack(3);
            m_currentCooldown = m_cooldownTime;

            std::cout << "Teleported enemy back!" << std::endl;
        }
    }
};

// 6. Self-Destruct Tower: 自爆 (AOE)
class SelfDestructTower : public Tower {
   public:
    SelfDestructTower(sf::Vector2f pos, auto& e, auto& p) : Tower(pos, e, p) {
        const auto& info = TowerData::INFO.at(TowerType::SelfDestruct);
        m_name = info.name;
        m_price = info.price;
        m_shape.setFillColor(info.color);  // 深灰

        m_range = 80.f;         // 觸發半徑
        m_damage = 500;         // 巨大傷害
        m_cooldownTime = 0.1f;  // 隨時準備

        // 爆炸特效 (圓形衝擊波)
        m_shockwave.setRadius(10.f);
        m_shockwave.setOrigin({10.f, 10.f});
        m_shockwave.setPosition(pos);
        m_shockwave.setFillColor(sf::Color::Transparent);
        m_shockwave.setOutlineColor(sf::Color(255, 100, 0, 255));  // 橘色
        m_shockwave.setOutlineThickness(5.f);
    }

    void update(sf::Time dt) override {
        // 如果正在爆炸，只處理動畫
        if (m_isExploding) {
            updateExplosion(dt);
            return;  // 跳過正常的 Tower update (不再冷卻或偵測敵人)
        }

        Tower::update(dt);
    }

    void draw(sf::RenderWindow& window) override {
        // 如果還沒爆炸，畫塔本體
        if (!m_isExploding) Tower::draw(window);
        // 如果正在爆炸，畫衝擊波
        else window.draw(m_shockwave);
    }

   protected:
    bool m_isExploding = false;
    sf::CircleShape m_shockwave;
    float m_explosionRadius = 150.f;  // 最大爆炸半徑
    float m_explosionAlpha = 255.f;   // 透明度

    void performAction() override {
        bool triggered = false;
        // 偵測觸發
        for (const auto& enemy : m_enemies) {
            if (enemy->isActive() && Utils::distance(getPosition(), enemy->getPosition()) <= m_range) {
                triggered = true;
                break;
            }
        }

        if (triggered) {
            // 1. 造成傷害 (邏輯)
            for (const auto& target : m_enemies) {
                if (target->isActive() && Utils::distance(getPosition(), target->getPosition()) <= m_explosionRadius) {
                    target->takeDamage(m_damage);
                }
            }

            // 2. 進入爆炸狀態 (視覺)
            m_isExploding = true;
            // 不需要 destroy()，等動畫跑完再 destroy
        }
    }

    void updateExplosion(sf::Time dt) {
        // 擴大半徑
        float currentRadius = m_shockwave.getRadius();
        float expansionSpeed = 300.f;  // 擴散速度
        float newRadius = currentRadius + expansionSpeed * dt.asSeconds();

        m_shockwave.setRadius(newRadius);
        m_shockwave.setOrigin({newRadius, newRadius});  // 保持中心對齊

        // 減少透明度 (Fade out)
        float fadeSpeed = 400.f;
        m_explosionAlpha -= fadeSpeed * dt.asSeconds();

        if (m_explosionAlpha <= 0.f) {
            m_explosionAlpha = 0.f;
            this->destroy();  // 動畫結束，真正移除物件
        }

        // 更新顏色
        sf::Color c = m_shockwave.getOutlineColor();
        c.a = static_cast<std::uint8_t>(m_explosionAlpha);
        m_shockwave.setOutlineColor(c);
    }
};